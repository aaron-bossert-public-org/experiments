
#pragma once

#include "igpu/buffer/vertex_parameter.h"

namespace igpu
{
    class gl_vertex_parameter : public vertex_parameter
    {
    public:
		
		gl_vertex_parameter(std::string, components, int location);
        
        int location() const;
        
		const vertex_parameter& parameter() const;

    private:
        
		int _location = -1;
    };
}
