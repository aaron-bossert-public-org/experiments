
#pragma once

#include "igpu/shader/primitive.h"

#include <unordered_map>

namespace igpu
{
	class fragment_shader;
	class parameter;
	class vertex_parameter;
	class vertex_shader;

    class program
    {
    public:
        
		struct config
		{
			std::string name;
			std::shared_ptr<vertex_shader> vertex;
			std::shared_ptr<fragment_shader> fragment;
		};

		virtual const config& cfg() const = 0;
		
		virtual ~program() = default;

		virtual size_t parameter_count() const = 0;
		
		virtual const parameter& parameter(size_t) const = 0;

		virtual size_t vertex_parameter_count() const = 0;

		virtual const vertex_parameter& vertex_parameter(size_t) const = 0;

    protected:
        
		program() = default;

    private:

        program (const program&) = delete;
        program& operator= (const program&) = delete;
    };
}

