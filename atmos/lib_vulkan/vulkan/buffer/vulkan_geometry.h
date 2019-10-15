
#pragma once

#include <igpu/buffer/geometry.h>
#include <igpu/buffer/vertex_format.h>

struct VkVertexInputAttributeDescription;

namespace igpu
{
	class vertex_constraints;

	class vulkan_geometry : public geometry
	{
	public:

		const config& cfg() const override;

		size_t element_start() const override;

		size_t element_count() const override;

		void element_start(size_t) override;

		void element_count(size_t) override;

		uint32_t vulkan_topology() const;

		const std::vector<VkVertexInputAttributeDescription>& vulkan_attribute_descriptions() const;

		static std::unique_ptr<vulkan_geometry> make(
			const config& cfg,
			const vertex_constraints&);

		~vulkan_geometry() override;

	protected:

		vulkan_geometry(
			const config&,
			unsigned vulkan_topology,
			std::vector<VkVertexInputAttributeDescription>);

	private:

		const config _cfg;
		size_t _element_start;
		size_t _element_count;

		const uint32_t _vulkan_topology;
		const std::vector<VkVertexInputAttributeDescription> _vulkan_attribute_descriptions;
	};
}
