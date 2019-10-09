
#pragma once

#include <igpu/buffer/geometry.h>
#include <igpu/buffer/vertex_format.h>

#include <vulkan/buffer/vulkan_index_resource.h>

struct VkVertexInputAttributeDescription;

namespace igpu
{
	class vertex_constraints;

	class vulkan_geometry : public geometry
	{
	public:

		uint32_t vulkan_topology() const;

		const std::vector<VkVertexInputAttributeDescription>& vulkan_attribute_descriptions() const;

		static std::unique_ptr<vulkan_geometry> make(
			const vertex_constraints&,
			std::string name,
			igpu::topology,
			size_t element_start,
			size_t element_count,
			std::shared_ptr<igpu::index_resource>,
			std::vector<std::shared_ptr<vertex_resource>>);

		~vulkan_geometry() override;

	protected:

		vulkan_geometry(
			std::string name,
			igpu::topology,
			size_t element_start,
			size_t element_count,
			unsigned vulkan_topology,
			std::shared_ptr<igpu::index_resource>,
			std::vector<std::shared_ptr<vertex_resource>>,
			std::vector<VkVertexInputAttributeDescription>);

	private:

		const uint32_t _vulkan_topology;

		const std::vector<VkVertexInputAttributeDescription> _vulkan_attribute_descriptions;
	};
}
