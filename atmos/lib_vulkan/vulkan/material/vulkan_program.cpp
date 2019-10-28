
#include <vulkan/material/vulkan_program.h>

#include <vulkan/context/vulkan_context.h>
#include <vulkan/material/vulkan_parameter.h>
#include <vulkan/material/vulkan_fragment_shader.h>
#include <vulkan/material/vulkan_vertex_shader.h>

#include <igpu/material/program_parsing.h>

#include <framework/logging/log.h>

#include <algorithm>
#include <array>

using namespace igpu;

//
////std::vector<VkVertexInputAttributeDescription> to_vulkan_attribute_descriptions(
//	const vertex_format::config& cfg)
//{
//	bool success = true;
//	const auto& attributes = cfg.attributes;
//	std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
//	attribute_descriptions.reserve(attributes.size());
//
//	for (int i = 0; i < attributes.size(); ++i)
//	{
//		const auto& attribute = attributes[i];
//		size_t location = vertex_constraints.find_location(attribute.parameter.name);
//
//		if (vertex_constraints::NOT_FOUND == location)
//		{
//			LOG_CRITICAL(
//				"in context vertex constraints could not find %s",
//				attribute.parameter.name.c_str());
//		}
//		else
//		{
//			const auto& found = vertex_constraints.cfg().vertex_parameters[location];
//			if (attribute.parameter.components != found.components)
//			{
//				LOG_CRITICAL("in context vertex constraints vertex parameter %s is expected to have components %s but vertex buffer is being created with %s instead",
//					attribute.parameter.name.c_str(),
//					to_string(found.components).data(),
//					to_string(attribute.parameter.components).data());
//			}
//			else
//			{
//				VkVertexInputAttributeDescription description;
//				description.location = (uint32_t)location;
//				description.binding = 0;
//				description.format = to_vulkan_format(attribute.parameter.components);
//				description.offset = attribute.offset;
//				attribute_descriptions.push_back(description);
//			}
//		}
//	}
//
//	if (!success)
//	{
//		return {};
//	}
//
//	return attribute_descriptions;
//}
////VkVertexInputBindingDescription to_vulkan_binding_description(
//	const vertex_format::config& cfg)
//{
//	VkVertexInputBindingDescription binding_description = {};
//	binding_description.binding = 0;
//	binding_description.stride = cfg.stride;
//	binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//	return binding_description;
//}
//

//------------------------------------------------------------------------------
//
//
const vulkan_program::config& vulkan_program::cfg() const
{
	return _cfg;
}

vulkan_program::~vulkan_program()
{
}

size_t vulkan_program::parameter_count() const
{
	return _parameters.size();
}

const vulkan_parameter& vulkan_program::parameter(size_t i) const
{
	return *_parameters[i];
}

size_t vulkan_program::batch_parameter_count() const
{
	return _batch_parameters.size();
}

size_t vulkan_program::vertex_parameter_count() const
{
	return _vertex_parameters.size();
}

const vulkan_vertex_parameter& vulkan_program::vertex_parameter(size_t i) const
{
	return _vertex_parameters[i];
}

const vulkan_parameter& vulkan_program::batch_parameter(size_t i) const
{
	return _batch_parameters[i];
}

size_t vulkan_program::material_parameter_count() const
{
	return _material_parameters.size();
}

const vulkan_parameter& vulkan_program::material_parameter(size_t i) const
{
	return _material_parameters[i];
}

size_t vulkan_program::instance_parameter_count() const
{
	return _instance_parameters.size();
}

const vulkan_parameter& vulkan_program::instance_parameter(size_t i) const
{
	return _instance_parameters[i];
}

std::unique_ptr<vulkan_program> vulkan_program::make(
	const config& cfg)
{
	std::vector<vulkan_parameter*> parameters;
	std::vector<vulkan_parameter> batch_parameters;
	std::vector<vulkan_parameter> material_parameters;
	std::vector<vulkan_parameter> instance_parameters;
	std::vector<vulkan_vertex_parameter> vertex_parameters;

	ASSERT_CONTEXT(false && "not implemented");

	//// sort vertex and shader parameters into batch/material/instance parameters by descriptor_set.
	//// tracked_params is used to avoid adding the same items multiple times if they exist in both vertex and fragment shader. 
	//// validate batch and material parameters using batch_constraints and material_constraints.
	//// validate instance parameters to ensure they are the same between vertex and fragment shader.
	//for (igpu::shader* shader : { (igpu::shader*)cfg.vertex.get(), (igpu::shader*)cfg.fragment.get() })
	//{
	//	auto vulkan = ASSERT_CAST(vulkan_shader*, shader);

	//	for (size_t i = 0; i < vulkan->program_parameter_count(); ++i)
	//	{
	//		const auto* param = &vulkan->program_parameter(i);

	//		const parameter_constraints* constraints = nullptr;
	//		std::vector<vulkan_parameter>* dest_parameters = nullptr;

	//		switch (param->descriptor_set)
	//		{
	//		case 0:
	//			constraints = &batch_constraints;
	//			dest_parameters = &batch_parameters;
	//			break;

	//		case 1:
	//			constraints = &material_constraints;
	//			dest_parameters = &material_parameters;
	//			break;

	//		case 2:
	//			dest_parameters = &instance_parameters;
	//			break;

	//		default:
	//			LOG_CRITICAL(
	//				"descriptor set index %d is out of range %d",
	//				(int)param->descriptor_set,
	//				(int)tracked_params.size());

	//			continue;
	//		}

	//		if (constraints)
	//		{
	//			if (param->binding >= constraints->cfg().parameters.size())
	//			{
	//				LOG_CRITICAL(
	//				"binding %d is out of range %d",
	//				(int)param->binding,
	//				(int)constraints->cfg().parameters.size());

	//				continue;
	//			}
	//			else
	//			{
	//				const primitive_kv& kv = constraints->cfg().parameters[param->binding];
	//				
	//				if (param->cfg.name != kv.first)
	//				{
	//					LOG_CRITICAL(
	//						"binding %d name mismatch, expected:%s actual:%s",
	//						(int)param->binding,
	//						param->cfg.name.c_str(),
	//						kv.first.c_str());

	//					continue;
	//				}
	//				else if (param->cfg.type != kv.second.type())
	//				{
	//					LOG_CRITICAL(
	//						"binding %s type mismatch, expected:%s actual:%s",
	//						param->cfg.name.c_str(),
	//						parameter::to_string(param->cfg.type).data(),
	//						parameter::to_string(kv.second.type()).data());

	//					continue;
	//				}
	//			}
	//		}
	//		
	//		bool& is_tracked = tracked_params[param->descriptor_set][param->binding];

	//		if (!is_tracked)
	//		{
	//			dest_parameters->push_back(vulkan_parameter(
	//				param->cfg,
	//				param->descriptor_set,
	//				param->binding));
	//		}
	//		
	//		if (!constraints)
	//		{
	//			if (param->cfg.name != kv.first)
	//			{
	//				LOG_CRITICAL(
	//					"binding %d name mismatch, expected:%s actual:%s",
	//					(int)param->binding,
	//					param->cfg.name.c_str(),
	//					kv.first.c_str());

	//				continue;
	//			}
	//			else if (param->cfg.type != kv.second.type())
	//			{
	//				LOG_CRITICAL(
	//					"binding %s type mismatch, expected:%s actual:%s",
	//					param->cfg.name.c_str(),
	//					parameter::to_string(param->cfg.type).data(),
	//					parameter::to_string(kv.second.type()).data());

	//				continue;
	//			}
	//		}

	//		is_tracked = true;
	//	}
	//}

	//// creat mapping of 
	//std::unordered_map<std::string_view, size_t> instance_parameter_lookup;
	//for (const auto& instance_parameter : instance_parameters)
	//{
	//	const std::string& name = instance_parameter.cfg().name;
	//	ASSERT_CONTEXT(instance_parameter_lookup.find(name) == instance_parameter_lookup.end());
	//	instance_parameter_lookup[name] = instance_parameter.binding();
	//}
	//
	
	ASSERT_CONTEXT(false && "need to implement vertex constraints here too");

	return std::unique_ptr<vulkan_program>(
		new vulkan_program(
			cfg,
			std::move(parameters),
			std::move(batch_parameters),
			std::move(material_parameters),
			std::move(instance_parameters),
			std::move(vertex_parameters)));
}

vulkan_program::vulkan_program(
	const config& cfg,
	std::vector<vulkan_parameter*> parameters,
	std::vector<vulkan_parameter> batch_parameters,
	std::vector<vulkan_parameter> material_parameters,
	std::vector<vulkan_parameter> instance_parameters,
	std::vector<vulkan_vertex_parameter> vertex_parameters)
	: _cfg(cfg)
	, _parameters(std::move(parameters))
	, _batch_parameters(std::move(batch_parameters))
	, _material_parameters(std::move(material_parameters))
	, _instance_parameters(std::move(instance_parameters))
	, _vertex_parameters(std::move(vertex_parameters))
	, _texture_switch_metric(perf::category::SWITCH_TEXTURES, "Shader Texture Switches")
	, _parameter_switch_metric(perf::category::SWITCH_PARAMETERS, "Shader parameter Switches")
{
}

