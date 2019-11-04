
#include "gfx/texture/texture_packer.h"

#include "framework/logging/log.h"
#include "framework/tweak/tweak.h"

#include "igpu/texture/texel_buffer_2d.h"
#include "igpu/texture/texture_2d.h"

#include "framework/perf/stack_timer.h"

#include <algorithm>

TWEAK_VARIABLE(debug_packed_textures, false, "All packed textures will black with white bordering textures to show worst case bleed");

TWEAK_VARIABLE(perf_no_packing_uv_twiddling, false, "If on, packed textures will use their original UVs");

using namespace gfx;

// texture packing logic is as follows:
//
// - split raw textures into raw atlases so that each raw atlas:
//     - contains textures of one internal texture format
//     - contains textures of one group name
//     - does not contain too many textures
// - sort raw textures in each raw atlas by size
//      - build an atlas texure from each raw atlas
//      - save uv information and a refrence to the atlas inside each packed texture request

namespace
{
	using requests_t = texture_packer::requests_t;

	bool is_pow2(int x)
	{
		return x && !(x & (x - 1));
	}

	int width_of_atlas_for_texel_count(int texel_count)
    {
		// the smallest square pow 2 texture that can hold the given number of texels
		int non_pow2_width = (int)std::ceil(std::sqrt(texel_count));

		// round up to the next pow 2
		int ceil_pow2 = non_pow2_width - 1;
		ceil_pow2 = ceil_pow2 | (ceil_pow2 >> 1);
		ceil_pow2 = ceil_pow2 | (ceil_pow2 >> 2);
		ceil_pow2 = ceil_pow2 | (ceil_pow2 >> 4);
		ceil_pow2 = ceil_pow2 | (ceil_pow2 >> 8);
		ceil_pow2 = ceil_pow2 | (ceil_pow2 >> 16);
		ceil_pow2 += 1;
		
		return ceil_pow2;
    }
    
    glm::ivec2 deinterleave (unsigned value)
    {
        // deinterleave transates morton order position to texel coordinate
        // this assumes square/powers of 2, and sub textures are sorted in atlases by size
        unsigned
        x = (value >> 1 ) & 0X55555555;
        x = (x | (x >>   1 )) & 0X33333333;
        x = (x | (x >>   2 )) & 0X0F0F0F0F;
        x = (x | (x >>   4 )) & 0X00FF00FF;
        x = (x | (x >>   8 )) & 0X0000FFFF;
        
        unsigned
        y = value & 0X55555555;
        y = (y | (y >>   1 )) & 0X33333333;
        y = (y | (y >>   2 )) & 0X0F0F0F0F;
        y = (y | (y >>   4 )) & 0X00FF00FF;
        y = (y | (y >>   8 )) & 0X0000FFFF;
        
        return glm::ivec2(x, y);
    }
    
    void copy_texture_data(const std::unique_ptr<char[]>& atlas_backing, int atlas_backing_size, const glm::ivec2& res, const texture_packer::msg& msg, const requests_t& requests)
    {
		size_t bytes_written = 0;
        char* atlas_data = &atlas_backing[0];
        
        if(msg.morton_order)
        {
            // morton order texure formats have implied perfect packing order for sorted square pow2 textures
            for(const auto& request : requests)
            {
                const char* texture_data = &request.raw().texture_data()[0];
                size_t texture_size = request.raw().texture_data().size();
                
                std::memcpy(atlas_data + bytes_written, texture_data, texture_size);
                
                bytes_written += texture_size;
            }
        }
        else
        {
            // non morton ordered must textures must be copied one block row at a time
            for(const auto& request : requests)
            {
				const char* texture_data = &request.raw().texture_data()[0];
                glm::ivec2 texture_blocks = request.raw().res() / msg.block_resolution;
                glm::ivec2 atlas_blocks = res / msg.block_resolution;
                glm::ivec2 atlas_blocks_offset = deinterleave(unsigned(bytes_written * msg.bits_per_pixel) / 8) / msg.block_resolution;
                
				int bytes_per_block = (msg.block_resolution.x * msg.block_resolution.y * msg.bits_per_pixel) / 8;
				int bytes_per_texture_row = texture_blocks.x * bytes_per_block;
				int bytes_per_atlas_row = atlas_blocks.x * bytes_per_block;
				int bytes_offset_atlas = atlas_blocks_offset.x * bytes_per_block + atlas_blocks_offset.y * bytes_per_atlas_row;
                
                for(int block_y = 0; block_y < texture_blocks.y; ++block_y)
                {
                    const char* src = texture_data + block_y * bytes_per_texture_row;
                    char* dst = atlas_data + bytes_offset_atlas + block_y * bytes_per_atlas_row;
                    
                    std::memcpy(dst, src, bytes_per_texture_row);
                    
                    bytes_written += bytes_per_texture_row;
                }
            }
        }
        
        ASSERT_CONTEXT(bytes_written <= atlas_backing_size);
    }
    
    void process_packed_info(bool transposed_uv,
                           const std::shared_ptr<igpu::texture_2d>& atlas,
                           const requests_t& requests)
    {
        unsigned texel_index = 0;
        for(const packing_request& request : requests)
        {
			const igpu::texel_buffer_2d& raw = request.raw();
			const std::shared_ptr<packed_texture> packed_texture = request.packed();
            
            if(!packed_texture)
            {
                LOG_CONTEXT( CRITICAL, "unable to process packing info, packed_texture is null for info(%s)", raw.path().c_str() );
            }
            else if(packed_texture->atlas())
            {
                LOG_CONTEXT( CRITICAL, "through weird twisty logic this packed texture has been processed multiple times (%s)", raw.path().c_str() );
            }
            else
            {
                auto uv_scale = raw.res() / atlas->res();
                auto uv_offset = deinterleave(texel_index) / atlas->res();
                
                if(transposed_uv) std::swap(uv_offset.x, uv_offset.y);
                
                if (perf_no_packing_uv_twiddling)
                {
                    uv_scale = glm::vec2(1.0f, 1.0f);
                    uv_offset = glm::vec2(0.0f, 0.0f);
                }
                
                packed_texture->process(atlas, uv_scale, uv_offset);
            }
            
            texel_index += raw.res().x * raw.res().y;
        }
    }
    
    // pack all the textures within a raw_atlas into a single atlas.
    // process the requests inside the raw_atlas with the atlas.
    void pack_raw_atlas(const texture_packer& texture_packer, const requests_t& requests, const int texel_count)
    {
        if(texel_count > 0)
        {
            auto name = " packed " +
            igpu::texel_buffer_2d::to_string(texture_packer.msg().format);
            
            STACK_TIMER_PAIR("pack_raw_atlas", name)
            
			int width = width_of_atlas_for_texel_count(texel_count);
            auto res = glm::vec2(width, width);
            
            // the most expensive part of creating an atlas is allocating the large buffer required to store the texels
            // 32.93 : pack_raw_atlas ( packed RGB ETC1 #1 diffuse) [2048x2048], 61.91% used, 37 textures
            // | 24.90 : make_backing
            // | 1.28 : copy_texture_data
            // | 5.64 : upload_atlas
            // | 0.12 : register_atlas_with_requests
            
            
			int atlas_backing_size = (width * width * texture_packer.msg().bits_per_pixel)/8;
            auto atlas_backing = std::unique_ptr<char[]>(new char[atlas_backing_size]);
            
            // copy the raw data into a single data buffer
            copy_texture_data(atlas_backing, atlas_backing_size, res, texture_packer.msg(), requests);
            
			std::shared_ptr<const igpu::texel_buffer_2d> raw = igpu::texel_buffer_2d::make(name, texture_packer.msg().format, res, std::move(atlas_backing), atlas_backing_size);
            
            // make a gpu texture out of packed data
			std::shared_ptr<igpu::texture_2d> atlas = igpu::texture_2d::make(*raw);
            
            // using texture atlas process all the packedrequests in the raw_atlas
            process_packed_info(texture_packer.msg().transposed_uv, atlas, requests);
        }
    }
    
    void debug_packed_texture_group(const requests_t& requests)
    {
        for(const auto& request : requests)
        {
            const int factor = 2;
            
            // width is the smallest square pow2 texture that can contain texels
            auto name = "debug_packed_texture(" + request.raw().path() + ")";
            
            // factor needs to be a power of 2
            const size_t size = request.raw().texture_data().size();
            const size_t  padded_size = size * factor * factor;
            auto data = std::unique_ptr<char[]>(new char[padded_size]);
            
            char* data_begin = data.get();
            char* data_end = data.get() + padded_size;
            
            std::fill(data_begin,
                      data_begin + size,
                      (char)0);
            
            std::fill(data_begin + size,
                      data_end,
                      (char)-1);
            
			auto raw = igpu::texel_buffer_2d::make(name, request.raw().format(), request.raw().res() * factor, std::move(data), padded_size);
            
            // make a gpu texture out of packed data
            std::shared_ptr<igpu::texture_2d> debug_texture =
				igpu::texture_2d::make(*raw);
            
            request.packed()->process(debug_texture, glm::ivec2(1, 1) / factor, glm::vec2(0, 0));
        }
    }
}

const texture_packer::msg& texture_packer::msg() const
{
    return _msg;
}

void texture_packer::process_requests(requests_t requests)
{
	requests.sort([](const packing_request& lhs, const packing_request& rhs) {
		return lhs.raw().texture_data().size() > rhs.raw().texture_data().size();
	});

	auto at = requests.begin();

	while (at != requests.end())
	{
		const size_t max_texel_count = _msg.max_atlas_resolution.x * _msg.max_atlas_resolution.y;
		int texel_count = 0;
		requests_t next_atlas_contents;

		// gather as many textures as can fit into one atlas
		while (at != requests.end())
		{
			const igpu::texel_buffer_2d& raw = at->raw();
			const glm::ivec2& res = raw.res();
			int request_texel_count = res.x * res.y;

			if (res.x > _msg.max_atlas_resolution.x || res.y > _msg.max_atlas_resolution.y)
			{
				LOG_CONTEXT(CRITICAL, "texture[%dx%d](%s) is larger than max allowed resolution[%dx%d]",
					raw.res().x,
					raw.res().y,
					raw.path().c_str(),
					_msg.max_atlas_resolution.x,
					_msg.max_atlas_resolution.y);
			}
			else if (_msg.morton_order && ((raw.res().x != raw.res().y) || !is_pow2(raw.res().x)))
			{
				LOG_CONTEXT(CRITICAL, "texture[%dx%d](%s) cannot be used for morton order texture format",
					raw.res().x,
					raw.res().y,
					raw.path().c_str());
			}
			else if (texel_count + request_texel_count > max_texel_count) {
				// atlas full, move on to actual texure packing 
				break;
			}
			else
			{
				texel_count += request_texel_count;
				next_atlas_contents.push_back(*at);
			}
			++at;
		}

		// actual texture packing
		if (debug_packed_textures)
		{
			debug_packed_texture_group(next_atlas_contents);
		}
		else
		{
			pack_raw_atlas(*this, next_atlas_contents, texel_count);
		}
	}
}

std::unique_ptr<texture_packer> texture_packer::make(const msg& msg)
{
    if(!msg.bits_per_pixel)
    {
        LOG_CONTEXT( CRITICAL, "bytes_per_texel is 0");
    }
    else if(0 == (msg.max_atlas_resolution.x * msg.max_atlas_resolution.y))
    {
        LOG_CONTEXT( 
			CRITICAL,
			"zero sized max atlas resolution(%d, %d)",
			msg.max_atlas_resolution.x,
			msg.max_atlas_resolution.y);
    }
    else if(0 == (msg.block_resolution.x * msg.block_resolution.y))
    {
        LOG_CONTEXT(
			CRITICAL,
			"zero sized block resolution(%d, %d)",
			msg.block_resolution.x,
			msg.block_resolution.y);
    }
    else
    {
        return std::unique_ptr<texture_packer>(new texture_packer(msg));
    }
    
    return nullptr;
}

texture_packer::texture_packer(const msg& msg)
: _msg(msg)
{
}


