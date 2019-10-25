
#pragma once

#include <igpu/buffer/buffer_usage.h>
#include <igpu/texture/texture_format.h>
#include <igpu/texture/sampler.h>

#include <glm/vec2.hpp>

namespace igpu
{
    class texture2d
    {
    public:

		ENUM_SERIALIZABLE(

			source_type, DEFAULT(UNDEFINED),
			(UNDEFINED, 0),
			(EXPLICIT, 1),
			(RAW_FILE, 2)
		);
		
		struct config
		{
			std::string name;
			sampler sampler;
			buffer_usage usage;
			bool can_auto_generate_mips = false;
		};

        virtual ~texture2d() = default;

		virtual void map(size_t, buffer_view_base*) = 0;

		virtual void unmap_explicit(
			const glm::ivec2&,
			texture_format) = 0;

		virtual void unmap_raw_file() = 0;

        virtual const config& cfg() const = 0;

        virtual source_type source() const = 0;

		virtual const glm::ivec2& res() const = 0;

		virtual texture_format format() const = 0;
        
    protected:
        
		texture2d() = default;
        texture2d (const texture2d&) = delete;
        texture2d& operator= (const texture2d&) = delete;
    };
}
