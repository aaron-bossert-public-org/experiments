
#include <vulkan/material/vulkan_program.h>

// Vulkan implementation includes - begin
#include <vulkan/context/vulkan_context.h>
#include <vulkan/material/vulkan_parameter.h>
// Vulkan implementation includes - end

#include <framework/logging/log.h>

#include <algorithm>

using namespace igpu;

//------------------------------------------------------------------------------
//
//
vulkan_program::~vulkan_program()
{
}

buffer_view<const parameter> vulkan_program::batch_parameters() const
{
	return buffer_view<const parameter>(
		_batch_parameters.size(),
		_batch_parameters.data(),
		sizeof(vulkan_parameter));
}

buffer_view<const parameter> vulkan_program::material_parameters() const
{
	return buffer_view<const parameter>(
		_material_parameters.size(),
		_material_parameters.data(),
		sizeof(vulkan_parameter));
}

buffer_view<const parameter> vulkan_program::instance_parameters() const
{
	return buffer_view<const parameter>(
		_instance_parameters.size(),
		_instance_parameters.data(),
		sizeof(vulkan_parameter));
}

size_t vulkan_program::index_of_instance_parameter(const std::string_view& name) const
{
	auto find = _instance_parameter_lookup.find(name);
	if (find != _instance_parameter_lookup.end())
	{
		return find->second;
	}

	return (size_t)-1;
}

const primitive& vulkan_program::default_instance_primitive(size_t instance_parameter_index) const
{
	static primitive default_compute = (primitive::variant_t)std::shared_ptr<compute_resource>();
	static primitive default_texture = (primitive::variant_t)std::shared_ptr<texture2d>();

	if (instance_parameter_index < _instance_parameters.size())
	{
		const parameter& instance_parameter = _instance_parameters[instance_parameter_index];
		parameter::type type = instance_parameter.cfg().type;
		switch (type)
		{
		case parameter::type::COMPUTE_RESOURCE:
			return default_compute;
		case parameter::type::TEXTURE2D:
			return default_texture;
		default:
			LOG_CONTEXT(CRITICAL, "unhandled type %s, value will not be reset before the next draw call", parameter::to_string(type).data());
		}
	}

	return default_texture;
}

std::unique_ptr<vulkan_program> vulkan_program::make(
	vulkan_context* vulkan_context,
	const buffer_view<uint8_t>& /*vertex_code*/,
	const buffer_view<uint8_t>& /*pixel_code*/)
{
	ASSERT_CONTEXT(false && "not implemented");

	std::vector<vulkan_parameter> parameters; //= discover_parameters(gl_handle);
	// discover_vertex_parameters(gl_handle);

	std::vector<vulkan_parameter> batch_parameters;
	std::vector<vulkan_parameter> material_parameters;
	std::vector<vulkan_parameter> instance_parameters;
	std::unordered_map<std::string_view, size_t> instance_parameter_lookup;

	for (size_t param_index = 0; param_index < parameters.size(); ++param_index)
	{
		vulkan_parameter& parameter = parameters[param_index];
		std::vector<vulkan_parameter>* dest_parameters;
		size_t constraint_index;

		const std::string_view& name = parameters[param_index].cfg().name;
		constraint_index = vulkan_context->batch_constraints().index_of(name);
		if (-1 != constraint_index)
		{
			dest_parameters = &batch_parameters;
		}
		else
		{
			constraint_index = vulkan_context->material_constraints().index_of(name);
			if (-1 != constraint_index)
			{
				dest_parameters = &material_parameters;
			}
			else
			{
				dest_parameters = &instance_parameters;
				constraint_index = instance_parameters.size();
			}
		}

		dest_parameters->push_back(vulkan_parameter(
			parameter.cfg(),
			constraint_index));
	}

	for (const auto& instance_parameter : instance_parameters)
	{
		const std::string& name = instance_parameter.cfg().name;
		ASSERT_CONTEXT(instance_parameter_lookup.find(name) == instance_parameter_lookup.end());
		instance_parameter_lookup[name] = instance_parameter.constraint_index();
	}

	return std::unique_ptr<vulkan_program>(
		new vulkan_program(
			vulkan_context,
			std::move(batch_parameters),
			std::move(material_parameters),
			std::move(instance_parameters),
			std::move(instance_parameter_lookup)));
}

vulkan_program::vulkan_program(
	vulkan_context* vulkan_context,
	std::vector<vulkan_parameter> batch_parameters,
	std::vector<vulkan_parameter> material_parameters,
	std::vector<vulkan_parameter> instance_parameters,
	std::unordered_map<std::string_view, size_t> instance_parameter_lookup)
	: _vulkan_context(vulkan_context)
	, _batch_parameters(std::move(batch_parameters))
	, _material_parameters(std::move(material_parameters))
	, _instance_parameters(std::move(instance_parameters))
	, _instance_parameter_lookup(instance_parameter_lookup)
	, _texture_switch_metric(perf::category::SWITCH_TEXTURES, "Shader Texture Switches")
	, _parameter_switch_metric(perf::category::SWITCH_PARAMETERS, "Shader parameter Switches")
{
}

