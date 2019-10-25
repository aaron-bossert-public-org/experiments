
#include <vulkan/buffer/vulkan_geometry.h>

#include <framework/logging/log.h>

#include <igpu/buffer/vertex_format.h>
#include <igpu/context/vertex_constraints.h>

using namespace igpu;

namespace
{
	VkPipelineVertexInputStateCreateInfo to_vulkan_vertex_input_info(
		const std::vector<VkVertexInputBindingDescription>& vertex_binding_descriptions,
		const std::vector<VkVertexInputAttributeDescription>& vertex_attribute_descriptions)
	{
		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = (uint32_t)vertex_binding_descriptions.size();
		vertex_input_info.vertexAttributeDescriptionCount = (uint32_t)vertex_attribute_descriptions.size();
		vertex_input_info.pVertexBindingDescriptions = vertex_binding_descriptions.data();
		vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();
		return vertex_input_info;
	}

	VkPipelineInputAssemblyStateCreateInfo to_vulkan_vertex_input_assembly_info(
		const geometry::config& cfg)
	{
		VkPipelineInputAssemblyStateCreateInfo vertex_input_assembly_info = {};
		vertex_input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		vertex_input_assembly_info.topology = to_vulkan_topology(cfg.topology);
		vertex_input_assembly_info.primitiveRestartEnable = VK_FALSE;
		return vertex_input_assembly_info;
	}
}

VkPrimitiveTopology igpu::to_vulkan_topology(topology topology)
{
	switch (topology)
	{
	case topology::POINT_LIST:
		return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
	case topology::LINE_LIST:
		return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
	case topology::LINE_STRIP:
		return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
	case topology::TRIANGLE_LIST:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	case topology::TRIANGLE_STRIP:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
	case topology::TRIANGLE_FAN:
		return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
	};

	if (is_valid(topology))
	{
		LOG_CRITICAL("unhandled format: %s", to_string(topology).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", topology);
	}

	return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
}

const vulkan_geometry::config& vulkan_geometry::cfg() const
{
	return _cfg;
}

size_t vulkan_geometry::element_start() const
{
	return _element_start.value_or(0);
}

void vulkan_geometry::element_start(size_t element_start)
{
	_element_start = element_start;
}

size_t vulkan_geometry::element_count() const
{
	return _element_count.has_value() ? _element_count.value() : _cfg.index_buffer->count();
}

void vulkan_geometry::element_count(size_t element_count)
{
	_element_count = element_count;
}

vulkan_index_buffer& vulkan_geometry::index_buffer()
{
	return *_index_buffer;
}

vulkan_vertex_buffer& vulkan_geometry::vertex_buffer(size_t i)
{
	return *_vertex_buffers[i];
}

const VkPipelineVertexInputStateCreateInfo& vulkan_geometry::vertex_input_info() const
{
	return _vertex_input_info;
}

const VkPipelineInputAssemblyStateCreateInfo& vulkan_geometry::vertex_input_assembly_info() const
{
	return _vertex_input_assembly_info;
}

std::unique_ptr<vulkan_geometry> vulkan_geometry::make(
	const config& cfg)
{
	if (nullptr == cfg.index_buffer)
	{
		LOG_CRITICAL("index buffer is null");
		return nullptr;
	}

	auto* vulkan_index_buffer = ASSERT_CAST(igpu::vulkan_index_buffer*, cfg.index_buffer.get());
	
	std::vector < vulkan_vertex_buffer* > vertex_buffers;
	std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions;
	std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions;
	
	vertex_buffers.reserve(cfg.vertex_buffers.size());
	vertex_binding_descriptions.reserve(cfg.vertex_buffers.size());
	vertex_attribute_descriptions.reserve(cfg.vertex_buffers.size());

	for (uint32_t i = 0; i < cfg.vertex_buffers.size(); ++i)
	{
		const auto& vertex_buffer = cfg.vertex_buffers[i];
		if (nullptr == vertex_buffer)
		{
			LOG_CRITICAL(
				"vertex buffer is null vertex_buffers[%d]",
				(int)i);

			return nullptr;
		}
		auto vulkan_vertex_buffer = ASSERT_CAST(igpu::vulkan_vertex_buffer*, vertex_buffer.get());
		auto binding_description = vulkan_vertex_buffer->binding_description();
		auto attribute_descriptions = vulkan_vertex_buffer->attribute_descriptions();

		binding_description.binding = i;
		for (auto& attribute_description : attribute_descriptions)
		{
			attribute_description.binding = i;
		}

		vertex_buffers.push_back(vulkan_vertex_buffer);
		vertex_binding_descriptions.emplace_back(binding_description);
		vertex_attribute_descriptions.insert(
			vertex_attribute_descriptions.end(),
			attribute_descriptions.begin(),
			attribute_descriptions.end());
	}

	return std::unique_ptr<vulkan_geometry>(
		new vulkan_geometry(
			cfg,
			vulkan_index_buffer,
			std::move(vertex_buffers),
			std::move(vertex_binding_descriptions),
			std::move(vertex_attribute_descriptions)));
}

vulkan_geometry::~vulkan_geometry()
{
}

vulkan_geometry::vulkan_geometry(
	const config& cfg,
	igpu::vulkan_index_buffer* const vulkan_index_buffer,
	std::vector < vulkan_vertex_buffer* > vertex_buffers,
	std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions,
	std::vector<VkVertexInputAttributeDescription> vertex_attribute_descriptions)
	: _cfg(cfg)
	, _index_buffer(vulkan_index_buffer)
	, _vertex_buffers(std::move(vertex_buffers))
	, _vertex_binding_descriptions(std::move(vertex_binding_descriptions))
	, _vertex_attribute_descriptions(std::move(vertex_attribute_descriptions))
	, _vertex_input_info(to_vulkan_vertex_input_info(_vertex_binding_descriptions, _vertex_attribute_descriptions))
	, _vertex_input_assembly_info(to_vulkan_vertex_input_assembly_info(cfg))
{
}
