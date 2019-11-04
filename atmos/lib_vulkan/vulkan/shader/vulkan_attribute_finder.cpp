#include "vulkan/shader/vulkan_attribute_finder.h"

#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_program.h"

#include "igpu/shader/attribute_finder.h"

//
//#include "igpu/buffer/vertex_buffer.h"
//#include "igpu/buffer/vertex_parameter.h"

using namespace igpu;

bool vulkan_attribute_finder::find_all_attributes(
	const vulkan_program& program,
	const vulkan_geometry& geometry)
{
	_binding_description_count = 0;
	attribute_finder attr_finder;
	if(!attr_finder.find_all_attributes(program, geometry))
	{
		return false;
	}

	std::array<VkVertexInputBindingDescription*, 16> binding_table = {};

	for (uint32_t i = 0; i < program.vertex_parameter_count(); ++i)
	{
		const auto& param = program.vertex_parameter(i);
		const auto& source = attr_finder.attribute_sources()[i];

		const auto& buff_cfg = geometry.cfg().vertex_buffers[source.buffer_index]->cfg();
		const auto& attr_cfg = buff_cfg.attributes[source.attribute_index];
		auto** pp_binding_description = &binding_table[source.buffer_index];

		if (!*pp_binding_description)
		{
			VkVertexInputBindingDescription* binding_description
				= *pp_binding_description
				= &_binding_descriptions[_binding_description_count];

			binding_description->binding = _binding_description_count;
			binding_description->stride = buff_cfg.stride;
			binding_description->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			_binding_description_count++;
		}

		VkVertexInputAttributeDescription* attribute_description = &_attribute_descriptions[i];
		attribute_description->binding = (*pp_binding_description)->binding;
		attribute_description->location = param.cfg().spv.location;
		attribute_description->format = param.cfg().vk.format;
		attribute_description->offset = attr_cfg.offset;
	}

	VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	vertex_input_info.vertexBindingDescriptionCount = _binding_description_count;
	vertex_input_info.pVertexBindingDescriptions = _binding_descriptions.data();

	vertex_input_info.vertexAttributeDescriptionCount = (uint32_t)program.vertex_parameter_count();
	vertex_input_info.pVertexAttributeDescriptions = _attribute_descriptions.data();

	return true;
}


uint32_t vulkan_attribute_finder::binding_description_count() const
{
	return _binding_description_count;
}

const VkVertexInputBindingDescription* vulkan_attribute_finder::binding_descriptions() const
{
	return _binding_descriptions.data();
}

const VkVertexInputAttributeDescription* vulkan_attribute_finder::attribute_descriptions() const
{
	return _attribute_descriptions.data();
}