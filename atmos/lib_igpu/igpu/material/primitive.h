
#pragma once

#include <igpu/material/parameter.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <string>
#include <variant>
#include <vector>

namespace igpu
{
	class primitive;
    class compute_resource;
    class depth_texture2d;
    class render_texture2d;
    class texture2d;

	using primitive_kv = std::pair<std::string, primitive>;

    class primitive
    {
    public:

		using variant_t = std::variant<
			std::shared_ptr<compute_resource>,
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

		void set(std::shared_ptr<compute_resource>);

		void set(std::shared_ptr<texture2d>);
		
		void set(std::shared_ptr<depth_texture2d>);
		
		void set(std::shared_ptr<render_texture2d>);

    private:

		variant_t _variant;
    };
}
