
#include <vulkan/buffer/vulkan_geometry.h>

#include <framework/logging/log.h>

// Vulkan implementation includes - begin
#include <vulkan/buffer/vulkan_index_resource.h>
#include <vulkan/buffer/vulkan_vertex_resource.h>
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

#include <igpu/buffer/vertex_format.h>
#include <igpu/context/vertex_constraints.h>

using namespace igpu;

namespace
{
	std::vector<VkVertexInputAttributeDescription> to_vulkan_attributes(
		const vertex_constraints& constraints,
		std::vector<std::shared_ptr<vertex_resource>> vertex_resources)
	{
		size_t total_count = 0;
		for (const auto& vertex_resource : vertex_resources)
		{
			if (vertex_resource)
			{
				total_count += vertex_resource->cfg().attributes.size();
			}
		}

		std::vector<VkVertexInputAttributeDescription> vulkan_attributes(total_count);
		auto at = vulkan_attributes.begin();

		for (uint32_t binding = 0; binding < (uint32_t)vertex_resources.size(); ++binding)
		{
			const auto& vertex_resource = vertex_resources[binding];

			if (vertex_resource)
			{
				for (const vertex_attribute& attribute : vertex_resource->cfg().attributes)
				{
					VkFormat vulkan_format = VK_FORMAT_UNDEFINED;
					uint32_t location = (uint32_t)constraints.find_location(attribute.parameter.name);
					const vertex_parameter ref_parameter = constraints.find_parameter(attribute.parameter.name);

					ASSERT_CONTEXT(
						attribute.parameter.components == ref_parameter.components,
						"attribute:%s components:%s does not match reference contained in vertex_constraints:%s",
						attribute.parameter.name.c_str(),
						to_string(attribute.parameter.components).data(),
						to_string(ref_parameter.components).data());

					switch (attribute.parameter.components)
					{
					case components::FLOAT1:
						vulkan_format = VK_FORMAT_R32_SFLOAT;
						break;
					case components::FLOAT2:
						vulkan_format = VK_FORMAT_R32G32_SFLOAT;
						break;
					case components::FLOAT3:
						vulkan_format = VK_FORMAT_R32G32B32_SFLOAT;
						break;
					case components::FLOAT4:
						vulkan_format = VK_FORMAT_R32G32B32A32_SFLOAT;
						break;
					default:
						LOG_CONTEXT(
							CRITICAL,
							"unhandled format: %s",
							to_string(attribute.parameter.components).data());
					};

					at->binding = binding;
					at->location = location;
					at->format = vulkan_format;
					at->offset = attribute.offset;
				}
			}
		}

		return vulkan_attributes;
	}
}

uint32_t vulkan_geometry::vulkan_topology() const
{
	return _vulkan_topology;
}

const std::vector<VkVertexInputAttributeDescription>& vulkan_geometry::vulkan_attribute_descriptions() const
{
	return _vulkan_attribute_descriptions;
}

std::unique_ptr<vulkan_geometry> vulkan_geometry::make(
	const vertex_constraints& constraints,
	std::string name,
	igpu::topology topology,
	size_t element_start,
	size_t element_count,
	std::shared_ptr<igpu::index_resource> index_resource,
	std::vector<std::shared_ptr<vertex_resource>> vertex_resources)
{
	if (nullptr == index_resource)
	{
		LOG_CONTEXT(CRITICAL,
			"index buffer is null");
		return nullptr;
	}

	for (size_t i = vertex_resources.size(); i-- > 0;)
	{
		const auto& vertex_resource = vertex_resources[i];
		if (nullptr == vertex_resource)
		{
			LOG_CONTEXT(CRITICAL,
				"vertex buffer is null vertex_resources[%d]",
				(int)i);

			return nullptr;
		}
	}

	VkPrimitiveTopology vulkan_topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	switch (topology)
	{
	case topology::TRIANGLE_LIST:
		vulkan_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	case topology::TRIANGLE_STRIP:
		vulkan_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	case topology::TRIANGLE_FAN:
		vulkan_topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
		break;
	case topology::LINE_LIST:
		vulkan_topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
		break;
	case topology::LINE_STRIP:
		vulkan_topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
		break;
	case topology::POINT_LIST:
		vulkan_topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		break;
	default:
		LOG_CONTEXT(CRITICAL, "unhandled topology: %s", to_string(topology).data());
		return nullptr;
	}

	auto vulkan_attribute_descriptions = to_vulkan_attributes(constraints, vertex_resources);

	return std::unique_ptr<vulkan_geometry>(
		new vulkan_geometry(
			std::move(name),
			topology,
			element_start,
			element_count,
			vulkan_topology,
			std::move(index_resource),
			std::move(vertex_resources),
			std::move(vulkan_attribute_descriptions)));
}

vulkan_geometry::~vulkan_geometry()
{
}

vulkan_geometry::vulkan_geometry(
	std::string name,
	igpu::topology topology,
	size_t element_start,
	size_t element_count,
	unsigned vulkan_topology,
	std::shared_ptr<igpu::index_resource> index_resource,
	std::vector<std::shared_ptr<vertex_resource>> vertex_resources,
	std::vector<VkVertexInputAttributeDescription> vulkan_attribute_descriptions)
	: geometry(
		std::move(name),
		topology,
		element_start,
		element_count,
		std::move(index_resource),
		std::move(vertex_resources))
	, _vulkan_topology(vulkan_topology)
	, _vulkan_attribute_descriptions(std::move(vulkan_attribute_descriptions))
{
}
