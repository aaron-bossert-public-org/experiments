#include <vulkan/batch/vulkan_vertex_mapper.h>


#include <vulkan/buffer/vulkan_geometry.h>
#include <vulkan/material/vulkan_program.h>

using namespace igpu;

vulkan_vertex_mapper::vulkan_vertex_mapper(
	const vulkan_program& program,
	const vulkan_geometry& geometry)
{
	// generating binding descriptions
	// This should be amazing performance assuming vertex buffers a layed out in a similar manner to their appearance in shaders.
	// worst case is n*m complexity for program vertx attributes and vertex buffers's attributes. also dynamic memory though which is great.

	bool success = true;
	size_t start_buff = 0;
	size_t start_attr = 0;
	uint32_t binding_count = 0;
	uint32_t attribute_count = 0;

	std::array< VkVertexInputBindingDescription*, 16> binding_table = {};

	for (size_t pram = 0; pram < program.vertex_parameter_count(); ++pram)
	{
		const vulkan_vertex_parameter& vertex_parameter = program.vertex_parameter(pram);
		const size_t count_buff = geometry.vertex_buffer_count();
		const vertex_buffer::attribute* found_attr = nullptr;
		size_t found_buff = 0;

		size_t buff = start_buff;
		size_t attr = start_attr;

		const vulkan_vertex_buffer* vertex_buffer = &geometry.vertex_buffer(buff);
		size_t count_attr = vertex_buffer->cfg().attributes.size();

		do
		{
			if (vertex_buffer->cfg().attributes[attr].parameter.name == vertex_parameter.cfg().name)
			{
				found_attr = &vertex_buffer->cfg().attributes[attr];
				found_buff = buff;
			}
			++attr;
			if (attr >= count_attr)
			{
				attr = 0;
				++buff;
				if (buff >= count_buff)
				{
					buff = 0;
					vertex_buffer = &geometry.vertex_buffer(0);
					count_attr = vertex_buffer->cfg().attributes.size();
				}
			}
		} while (!found_attr && (buff != start_buff || attr != start_attr));

		start_buff = buff;
		start_attr = attr;

		if (!found_attr)
		{
			success = false;
			LOG_CRITICAL(
				"program (%s) "
				"expects geometry (%s) "
				"to contain attribute (%s) but could not find it",
				program.cfg().name.c_str(),
				geometry.cfg().name.c_str(),
				vertex_parameter.cfg().name.c_str());
		}
		else
		{
			// insert vertex buffer descriptions into bindings lazily (avoid adding the ones not needed)
			VkVertexInputBindingDescription** p_binding = &binding_table[found_buff];
			if (!*p_binding)
			{
				*p_binding = &_bindings[binding_count++];
				VkVertexInputBindingDescription* binding = *p_binding;
				*binding = {};
				binding->binding = 0;
				binding->stride = geometry.vertex_buffer(found_buff).cfg().stride;
				binding->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			}

			if (vertex_parameter.cfg().components != found_attr->parameter.components)
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
					to_string(found_attr->parameter.components).data());
			}

			VkVertexInputAttributeDescription* attribute = &_attributes[attribute_count++];
			(*attribute) = {};
			attribute->binding = (uint32_t)(*p_binding - &_bindings[0]);
			attribute->location = vertex_parameter.cfg().spv.location;
			attribute->format = vertex_parameter.cfg().vk.format;
			attribute->offset = found_attr->offset;
		}

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