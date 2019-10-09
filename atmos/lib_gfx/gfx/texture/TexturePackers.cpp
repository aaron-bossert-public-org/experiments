
#include <gfx/texture/texture_packers.h>

#include <framework/logging/log.h>
#include <framework/perf/stack_timer.h>

#include <gfx/texture/texture_packer.h>

using namespace gfx;

void texture_packers::process_requests(requests_t& requests)
{
	// split up requests by texture format
	std::unordered_map<igpu::texture_format, texture_packer::requests_t> map;
    for(auto& request : requests)
    {
		const igpu::texel_buffer_2d&  raw = request.raw();
        auto format = raw.msg().format;
            
		map[format].emplace_back(request);
	}
    
	// process each list of requests for each format
    for(auto& pair : map)
    {
		auto format = pair.first;
		auto find = _packers.find(format);

		if (find == _packers.end())
		{
			for (const auto& request : pair.second)
			{
				// texture packer not found for given format, load the texel buffer into a non packed texture.
				LOG_CONTEXT(INFORM, "could not find packer for format %s, texture:%s",
					igpu::to_string(format).data(), request.raw().msg().name.c_str());
				request.packed()->process(igpu::texture_2d::make(request.raw()), glm::vec2(1, 1), glm::vec2(0, 0));
			}
		}
		else
		{
			find->second->process_requests(std::move(pair.second));
		}
    }
}

texture_packers::~texture_packers()
{
}

std::unique_ptr<texture_packers> texture_packers::make()
{
	texture_packer::msg::flags pvr_xy = texture_packer::msg::flags::MORTON;
	texture_packer::msg::flags pvr_yx = texture_packer::msg::flags(texture_packer::msg::flags::MORTON | texture_packer::msg::flags::TRANSPOSED);

	texture_packer::msg packer_args[] = {
		{ igpu::texture_format::INT_A8, 8, glm::ivec2(1) },
		{ igpu::texture_format::INT_R8A8, 16, glm::ivec2(1) },
		{ igpu::texture_format::INT_R8G8B8, 24, glm::ivec2(1) },
		{ igpu::texture_format::INT_R8G8B8A8, 32, glm::ivec2(1) },

		{ igpu::texture_format::INT_A16, 16, glm::ivec2(1) },
		{ igpu::texture_format::INT_R16A16, 32, glm::ivec2(1) },
		{ igpu::texture_format::INT_R16G16B16, 48, glm::ivec2(1) },
		{ igpu::texture_format::INT_R16G16B16A16, 64, glm::ivec2(1) },

		{ igpu::texture_format::PVR2_4BPP, 4, glm::ivec2(4), pvr_xy },
		{ igpu::texture_format::PVR1_4BPP_RGB, 4, glm::ivec2(4), pvr_xy },
		{ igpu::texture_format::PVR1_4BPP_RGBA, 4, glm::ivec2(4), pvr_xy },

		{ igpu::texture_format::PVR2_2BPP, 2, glm::ivec2(4), pvr_yx },
		{ igpu::texture_format::PVR1_2BPP_RGB, 2, glm::ivec2(4), pvr_yx },
		{ igpu::texture_format::PVR1_2BPP_RGBA, 2, glm::ivec2(4), pvr_yx },

		{ igpu::texture_format::ETC1_RGB, 8, glm::ivec2(4) },

		{ igpu::texture_format::DXT_1_RGB, 4, glm::ivec2(4) },
		{ igpu::texture_format::DXT_1_RGBA, 4, glm::ivec2(4) },
		{ igpu::texture_format::DXT_3_RGBA, 8, glm::ivec2(4) },
		{ igpu::texture_format::DXT_5_RGBA, 8, glm::ivec2(4) },
	};
	
    packers_t packers;
    for(const auto& msg : packer_args)
    {
		auto packer = texture_packer::make(msg);
        if(packer)
        {
            packers.emplace(packer->msg().format, std::move(packer));
        }
    }
    
    return std::unique_ptr<texture_packers>(new texture_packers(std::move(packers)));
}

texture_packers::texture_packers(packers_t&& packers)
: _packers(std::move(packers))
{
}
