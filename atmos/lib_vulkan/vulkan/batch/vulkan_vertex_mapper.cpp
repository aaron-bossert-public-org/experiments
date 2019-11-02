#include <vulkan/batch/vulkan_vertex_mapper.h>


#include <vulkan/buffer/vulkan_geometry.h>
#include <vulkan/shader/vulkan_program.h>

using namespace igpu;

vulkan_vertex_mapper::vulkan_vertex_mapper(
	const vulkan_program& program,
	const vulkan_geometry& geometry)
{
	// generating binding descriptions
	// This should be amazing performance assuming vertex buffers a layed out in a similar manner to their appearance in shaders.
	// worst case is n*m complexity for program vertx attributes and vertex buffers's attributes. also dynamic memory though which is great.

	bool success = true;
	size_t expect_buff = 0;
	size_t expect_attr = 0;
	uint32_t binding_count = 0;
	uint32_t attribute_count = 0;

	std::array< VkVertexInputBindingDescription*, 16> binding_table = {};

	for (size_t pram = 0; pram < program.vertex_parameter_count(); ++pram)
	{
		const vulkan_vertex_parameter& vertex_parameter = program.vertex_parameter(pram);

		if (!geometry.find_expected_vertex_param(vertex_parameter.cfg().name, &expect_buff, &expect_attr))
		{
			success = false;
			LOG_CRITICAL(
				"program (%s) "
				"expects geometry (%s) "
				"to contain attribute (%s) but could not find it",
				program.cfg().name.c_str(),
				geometry.cfg().name.c_str(),
				vertex_parameter.cfg().name.c_str());
			continue;
		}

		size_t buff = expect_buff++;
		size_t attr = expect_attr++;
		const vertex_buffer& vertex_buffer = geometry.vertex_buffer(buff);
		const vertex_buffer::attribute& attribute = vertex_buffer.cfg().attributes[attr];

		// insert vertex buffer descriptions into bindings lazily (avoid adding the ones not needed)
		VkVertexInputBindingDescription** p_binding = &binding_table[buff];
		if (!*p_binding)
		{
			*p_binding = &_bindings[binding_count++];
			VkVertexInputBindingDescription* binding = *p_binding;
			*binding = {};
			binding->binding = (uint32_t)buff;
			binding->stride = vertex_buffer.cfg().stride;
			binding->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		}

		if (vertex_parameter.cfg().components != attribute.parameter.components)
		{
			success = false;
			LOG_CRITICAL(
				"program (%s) "
				"expects geometry (%s) "
				"attribute (%s) "
				"to be (%s), but found ",
				"(%s) here instead",
				program.cfg().name.c_str(),
				geometry.cfg().name.c_str(),
				vertex_parameter.cfg().name.c_str(),
				to_string(vertex_parameter.cfg().components).data(),
				to_string(attribute.parameter.components).data());
		}

		VkVertexInputAttributeDescription* p_attr_desc = &_attributes[attribute_count++];
		(*p_attr_desc) = {};
		p_attr_desc->binding = (uint32_t)buff;
		p_attr_desc->location = vertex_parameter.cfg().spv.location;
		p_attr_desc->format = vertex_parameter.cfg().vk.format;
		p_attr_desc->offset = attribute.offset;
	}

	if (success)
	{
		VkPipelineVertexInputStateCreateInfo info = {};

		info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		info.vertexBindingDescriptionCount = binding_count;
		info.vertexAttributeDescriptionCount = attribute_count;
		info.pVertexBindingDescriptions = &_bindings[0];
		info.pVertexAttributeDescriptions = &_attributes[0];

		_results = info;
	}
}


const VkPipelineVertexInputStateCreateInfo* vulkan_vertex_mapper::results() const
{
	if (_results.has_value())
	{
		return &_results.value();
	}

	return nullptr;
}