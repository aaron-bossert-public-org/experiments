
#pragma once

#include <igpu/material/vertex_shader.h>

#include <memory>

namespace igpu
{   
    class gl_vertex_shader : public vertex_shader
    {
    public:

		virtual unsigned gl_handle() const = 0;

        static std::unique_ptr<gl_vertex_shader> make(const config&);
    };
}
