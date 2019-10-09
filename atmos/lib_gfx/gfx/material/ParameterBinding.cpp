
#include <gfx/material/parameter_binding.h>

#include <igpu/material/parameter.h>
#include <igpu/material/program.h>
#include <igpu/texture/texture_2d.h>
#include <igpu/texture/render_texture_2d.h>

using namespace gfx

void parameter_binding::set(const std::string& name, const igpu::primitive* primitive, const igpu::program& program)
{
    _name = name;
    _primitive = primitive;
    _parameter = program.lookup(_name);
}

const std::string& parameter_binding::name() const
{
    return _name;
}

const igpu::primitive* parameter_binding::primitive() const
{
    return _primitive;
}

const igpu::parameter* parameter_binding::parameter() const
{
    return _parameter;
}
