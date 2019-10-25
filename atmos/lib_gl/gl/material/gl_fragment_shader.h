
#pragma once

#include <igpu/material/fragment_shader.h>

#include <memory>

namespace igpu
{   
    class gl_fragment_shader : public fragment_shader
    {
    public:

		virtual unsigned gl_handle() const = 0;

        static std::unique_ptr<gl_fragment_shader> make();
    };
}
