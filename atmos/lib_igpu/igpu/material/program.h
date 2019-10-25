
#pragma once

#include <igpu/material/primitive.h>

#include <unordered_map>

namespace igpu
{
	class vertex_shader;
	class fragment_shader;

	class parameter;

	class shaders
	{
	public:
		std::shared_ptr<vertex_shader> vertex;
		std::shared_ptr<fragment_shader> fragment;
	};

    class program
    {
    public:
        
		virtual ~program() = default;

		virtual size_t batch_parameter_count() const = 0;
		virtual const parameter& batch_parameter(size_t) const = 0;

		virtual size_t material_parameter_count() const = 0;
		virtual const parameter& material_parameter(size_t) const = 0;

		virtual size_t instance_parameter_count() const = 0;
		virtual const parameter& instance_parameter(size_t) const = 0;

		virtual size_t index_of_instance_parameter(const std::string_view&) const = 0;

		virtual const primitive& default_instance_primitive(size_t instance_parameter_index) const = 0;

    protected:
        
		program() = default;

    private:

        program (const program&) = delete;
        program& operator= (const program&) = delete;
    };
}

