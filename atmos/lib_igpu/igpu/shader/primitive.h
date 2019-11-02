
#pragma once

#include <igpu/shader/parameter.h>

#include <string>
#include <variant>

namespace igpu
{
	class compute_buffer;
	class texture2d;
	class depth_texture2d;
	class render_texture2d;

    class primitive
    {
    public:
		
		using variant_t = std::variant<
			std::shared_ptr<compute_buffer>,
			std::shared_ptr<texture2d>,
			std::shared_ptr<depth_texture2d>,
			std::shared_ptr<render_texture2d>>;
	
		struct config
		{
			std::string name;
			variant_t value;
		};

		virtual const config& cfg() const = 0;

		parameter::type type() const;
    };
}
