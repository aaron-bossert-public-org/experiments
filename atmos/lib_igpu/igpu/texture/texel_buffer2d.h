
#pragma once

#include <framework/meta_programming/va_macro.h>
#include <framework/utility/buffer_view.h>
#include <igpu/texture/sampler.h>
#include <igpu/texture/texture_format.h>
#include <glm/vec2.hpp>
#include <memory>
#include <string>

namespace igpu
{
    class texel_buffer2d
    {
    public:
   		struct config
		{
			std::string name;
			sampler sampler;
			glm::ivec2 res;
			texture_format format = texture_format::DEFAULT;
		};

		const config& cfg() const;
        
        const buffer_view<char>& texture_data() const;
       
		texel_buffer2d(
			const config&,
			const buffer_view<char>& texture_data,
			std::shared_ptr<const char[]> intermediate_data = nullptr);

		static std::unique_ptr<const texel_buffer2d> make(
			const std::string& name,
			const sampler& sampler,
			buffer_view<char> raw_file_data);

    private:
        
		const config _cfg;
		buffer_view<char> _texture_data;
        std::shared_ptr<const char[]> _intermediate_data;
    };
}
