
#pragma once

#include <igpu/texture/texture_format.h>
#include <gfx/texture/packed_texture.h>

#include <igpu/texture/texel_buffer_2d.h>

#include <list>

namespace gfx
{
    class texture_packer
    {
    public:
        
        using requests_t = std::list<packing_request>;
        
        struct msg
        {
			enum flags : uint32_t
			{
				NONE = 0,
				TRANSPOSED = 1 << 0,
				MORTON = 1 << 1,
			};
			igpu::texture_format format;
            uint32_t bits_per_pixel;
            glm::ivec2 block_resolution;
            flags flags = flags::NONE;
            uint32_t max_atlas_resolution = 2048;
        };
        
        const msg& msg() const;
        
        void process_requests(requests_t);
        
        static std::unique_ptr<texture_packer> make(const msg&);
        
        texture_packer(const msg&);
        
    private:
        
        const msg _msg;
    };
}
