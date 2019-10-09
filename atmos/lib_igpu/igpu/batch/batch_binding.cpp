
#include <igpu/batch/batch_binding.h>

#include <framework/utility/buffer_view.h>

#include <igpu/material/primitive.h>
#include <igpu/material/program.h>
#include <igpu/material/material.h>

using namespace igpu;
namespace
{
	primitive_block make_batch_binding_primitive_block(const batch_binding::config& cfg)
	{
		const auto& program = cfg.material->cfg().program;

		primitive_block::get_index_of_t get_index_of = [program](const std::string_view& name) { 
			return program->index_of_instance_parameter(name); 
		};

		primitive_block::get_default_t get_default = [program](size_t instance_parameter_index) -> const primitive& {
			return program->default_instance_primitive(instance_parameter_index);
		};

		return primitive_block(get_index_of, get_default, program->instance_parameters().size());
	}
}

const batch_binding::config& batch_binding::cfg() const
{
    return _cfg;
}

const utility::sphere& batch_binding::visibility_sphere() const
{
    return _visibility_sphere;
}

void batch_binding::visibility_sphere(const utility::sphere& visibility_sphere)
{
    _visibility_sphere = visibility_sphere;
}

const igpu::primitive_block& batch_binding::primitive_block() const
{
	return _primitive_block;
}

batch_binding::batch_binding(
	const config& cfg,
	const utility::sphere& visibility_sphere)
: _cfg(cfg)
, _visibility_sphere(visibility_sphere)
, _primitive_block(make_batch_binding_primitive_block(cfg))
{
}