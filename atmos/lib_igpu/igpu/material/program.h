
#pragma once

#include <igpu/material/primitive.h>

#include <unordered_map>

template <typename T>
class buffer_view;

namespace igpu
{
	class parameter;

    class program
    {
    public:
        
		virtual ~program();
        
		virtual buffer_view<const parameter> batch_parameters() const = 0;
		virtual buffer_view<const parameter> material_parameters() const = 0;
		virtual buffer_view<const parameter> instance_parameters() const = 0;
		virtual size_t index_of_instance_parameter(const std::string_view&) const = 0;
		virtual const primitive& default_instance_primitive(size_t instance_parameter_index) const = 0;

    protected:
        
        program();

    private:

        program (const program&) = delete;
        program& operator= (const program&) = delete;
    };
}

