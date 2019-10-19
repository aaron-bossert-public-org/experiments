
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <igpu/context/vertex_constraints.h>

using namespace igpu;

namespace
{
	std::vector<VkVertexInputAttributeDescription> to_vulkan_descriptions(
		const vertex_format::config& cfg,
		const vertex_constraints& vertex_constraints)
	{
		bool success = true;
		const auto& attributes = cfg.attributes;
		std::vector<VkVertexInputAttributeDescription> attribute_descriptions;
		attribute_descriptions.reserve(attributes.size());
		
		for (int i = 0; i < attributes.size(); ++i)
		{
			const auto& attribute = attributes[i];
			size_t location = vertex_constraints.find_location(attribute.parameter.name);

			if (vertex_constraints::NOT_FOUND == location)
			{
				LOG_CRITICAL(
					"in context vertex constraints could not find %s",
					attribute.parameter.name.c_str());
			}
			else
			{
				const auto& found = vertex_constraints.cfg().vertex_parameters[location];
				if (attribute.parameter.components != found.components)
				{
					LOG_CRITICAL("in context vertex constraints vertex parameter %s is expected to have components %s but vertex buffer is being created with %s instead",
						attribute.parameter.name.c_str(),
						to_string(found.components).data(),
						to_string(attribute.parameter.components).data());
				}
				else
				{
					VkVertexInputAttributeDescription description;
					description.location = (uint32_t)location;
					description.binding = 0;
					description.format = to_vulkan_format(attribute.parameter.components);
					description.offset = attribute.offset;
					attribute_descriptions.push_back(description);
				}
			}
		}

		if (!success)
		{
			return {};
		}

		return attribute_descriptions;
	}
}


VkFormat igpu::to_vulkan_format(components format)
{
	switch (format)
	{
	case components::FLOAT1:
		return VK_FORMAT_R32_SFLOAT;
	case components::FLOAT2:
		return VK_FORMAT_R32G32_SFLOAT;
	case components::FLOAT3:
		return VK_FORMAT_R32G32B32_SFLOAT;
	case components::FLOAT4:
		return VK_FORMAT_R32G32B32A32_SFLOAT;
	};

	if (is_valid(format))
	{
		LOG_CRITICAL("unhandled format: %s", to_string(format).data());
	}
	else
	{
		LOG_CRITICAL("invalid format: %d", format);
	}

	return VK_FORMAT_UNDEFINED;
}

std::unique_ptr<vulkan_vertex_buffer> vulkan_vertex_buffer::make(
	const config& cfg,
	const scoped_ptr < vulkan_buffer_mediator >& buffer_mediator,
	const vertex_constraints& vertex_constraints)
{
	vulkan_staged_buffer::config buffer_cfg({
		cfg.usage,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		buffer_mediator });

	auto attribute_descriptions = to_vulkan_descriptions(cfg.format, vertex_constraints);

	if (cfg.format.attributes.size() == 0)
	{
		LOG_CRITICAL("no attributes specified");
	}
	else if (cfg.format.attributes.size() == attribute_descriptions.size())
	{
		if (vulkan_staged_buffer::validate(buffer_cfg))
		{
			return std::unique_ptr<vulkan_vertex_buffer>(
				new vulkan_vertex_buffer(
					cfg, 
					buffer_cfg,
					attribute_descriptions));
		}
	}

	return nullptr;
}

const vulkan_vertex_buffer::config& vulkan_vertex_buffer::cfg() const
{
	return _cfg;
}

const std::vector<VkVertexInputAttributeDescription>& vulkan_vertex_buffer::attribute_descriptions() const
{
	return _attribute_descriptions;
}

vulkan_vertex_buffer::vulkan_vertex_buffer(
	const config& cfg,
	const vulkan_staged_buffer::config& buffer_cfg,
	const std::vector<VkVertexInputAttributeDescription>& attribute_descriptions)
	: vulkan_staged_buffer_t (buffer_cfg)
	, _cfg(cfg)
	, _attribute_descriptions(attribute_descriptions)
{
}
