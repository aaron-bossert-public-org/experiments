
#include <vulkan/buffer/vulkan_geometry.h>

#include <framework/logging/log.h>

// Vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/vertex_buffer.h>
#include <igpu/buffer/vertex_format.h>
#include <igpu/context/vertex_constraints.h>

using namespace igpu;

namespace
{
	std::vector<VkVertexInputAttributeDescription> to_vulkan_attributes(
		const vertex_constraints& constraints,
		std::vector<std::shared_ptr<vertex_buffer>> vertex_buffers)
	{
		size_t total_count = 0;
		for (const auto& vertex_buffer : vertex_buffers)
		{
			if (vertex_buffer)
			{
				total_count += vertex_buffer->cfg().format.attributes.size();
			}
		}

		std::vector<VkVertexInputAttributeDescription> vulkan_attributes(total_count);
		auto at = vulkan_attributes.begin();

		for (uint32_t binding = 0; binding < (uint32_t)vertex_buffers.size(); ++binding)
		{
			const auto& vertex_buffer = vertex_buffers[binding];

			if (vertex_buffer)
			{
				for (const vertex_attribute& attribute : vertex_buffer->cfg().format.attributes)
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
						LOG_CRITICAL("unhandled format: %s",
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

const vulkan_geometry::config& vulkan_geometry::cfg() const
{
	return _cfg;
}

size_t vulkan_geometry::element_start() const
{
	return _element_start;
}

void vulkan_geometry::element_start(size_t element_start)
{
	_element_start = element_start;
}

size_t vulkan_geometry::element_count() const
{
	return _element_count;
}

void vulkan_geometry::element_count(size_t element_count)
{
	_element_count = element_count;
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
	const config& cfg,
	const vertex_constraints& constraints)
{
	if (nullptr == cfg.index_buffer)
	{
		LOG_CRITICAL("index buffer is null");
		return nullptr;
	}

	for (size_t i = 0; i < cfg.vertex_buffers.size(); ++i)
	{
		const auto& vertex_buffer = cfg.vertex_buffers[i];
		if (nullptr == vertex_buffer)
		{
			LOG_CRITICAL(
				"vertex buffer is null vertex_buffers[%d]",
				(int)i);

			return nullptr;
		}
	}

	VkPrimitiveTopology vulkan_topology = VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	switch (cfg.topology)
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
		LOG_CRITICAL("unhandled topology: %s",
			to_string(cfg.topology).data());
		return nullptr;
	}

	auto vulkan_attribute_descriptions = to_vulkan_attributes(constraints, cfg.vertex_buffers);

	return std::unique_ptr<vulkan_geometry>(
		new vulkan_geometry(
			cfg,
			vulkan_topology,
			std::move(vulkan_attribute_descriptions)));
}

vulkan_geometry::~vulkan_geometry()
{
}

vulkan_geometry::vulkan_geometry(
	const config& cfg,
	unsigned vulkan_topology,
	std::vector<VkVertexInputAttributeDescription> vulkan_attribute_descriptions)
	: _cfg(cfg)
	, _element_start(0)
	, _element_count(cfg.index_buffer->count())
	, _vulkan_topology(vulkan_topology)
	, _vulkan_attribute_descriptions(std::move(vulkan_attribute_descriptions))
{
}
