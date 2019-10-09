
#pragma once

#include <igpu/texture/texel_buffer2d.h>

namespace igpu
{
    class texture2d
    {
    public:
		struct config
		{
			std::string name;
			sampler sampler;
			glm::ivec2 res;
			texture_format format = texture_format::DEFAULT;
		};
        
        virtual ~texture2d() {}
        
        const texel_buffer2d::config& cfg() const;
        
		size_t size() const;
        
    protected:
        
        texture2d(
			const texel_buffer2d::config&,
            const size_t size);
        
		texel_buffer2d::config _cfg;
        const size_t _size;
        
        texture2d (const texture2d&) = delete;
        texture2d& operator= (const texture2d&) = delete;
    };
}
