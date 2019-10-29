
#pragma once

#include <igpu/material/parameter.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <variant>
#include <vector>

namespace igpu
{
    class compute_buffer;
    class depth_texture2d;
	class primitive;
	class primitive_kv;
    class render_texture2d;
    class texture2d;

    class primitive
    {
    public:

		using variant_t = std::variant<
			std::shared_ptr<compute_buffer>,
			std::shared_ptr<texture2d>,
			std::shared_ptr<depth_texture2d>,
			std::shared_ptr<render_texture2d>>;

		primitive(primitive&&) = default;
		
		primitive(const primitive&) = default;

		primitive(variant_t);

		primitive& operator=(primitive&&);

		primitive& operator=(const primitive&);
		
		parameter::type type() const;

		const variant_t& variant() const;

		void set(std::shared_ptr<compute_buffer>);

		void set(std::shared_ptr<texture2d>);
		
		void set(std::shared_ptr<depth_texture2d>);
		
		void set(std::shared_ptr<render_texture2d>);

    private:

		variant_t _variant;
    };

	class primitive_kv : public std::pair<std::string, primitive>
	{
	};
}
