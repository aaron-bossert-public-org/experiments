
#pragma once

#include <vulkan/buffer/vulkan_index_buffer.h>
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <vulkan/defines/vulkan_includes.h>

#include <igpu/buffer/geometry.h>

#include <optional>

namespace igpu
{
	VkPrimitiveTopology to_vulkan_topology(topology topology);
	
	class vertex_constraints;

	class vulkan_geometry : public geometry
	{
	public:

		const config& cfg() const override;

		size_t element_start() const override;

		size_t element_count() const override;

		void element_start(size_t) override;

		void element_count(size_t) override;

		vulkan_index_buffer& index_buffer() override;
		
		vulkan_vertex_buffer& vertex_buffer(size_t index) override;

		const VkPipelineVertexInputStateCreateInfo& vertex_input_info() const;
		
		const VkPipelineInputAssemblyStateCreateInfo& vertex_input_assembly_info() const;

		static std::unique_ptr<vulkan_geometry> make(
			const config&);

		~vulkan_geometry() override;

	protected:

		vulkan_geometry(
			const config&,
			igpu::vulkan_index_buffer* const,
			std::vector < vulkan_vertex_buffer* >,
			std::vector<VkVertexInputBindingDescription> vertex_binding_descriptions,
			std::vector<VkVertexInputAttributeDescription> vertex_attribute_description);

	private:

		const config _cfg;
		igpu::vulkan_index_buffer* const _index_buffer = nullptr;
		const std::vector < vulkan_vertex_buffer* > _vertex_buffers;
		const std::vector<VkVertexInputBindingDescription> _vertex_binding_descriptions;
		const std::vector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions;
		const VkPipelineVertexInputStateCreateInfo _vertex_input_info = {};
		const VkPipelineInputAssemblyStateCreateInfo _vertex_input_assembly_info = {};

		std::optional<size_t> _element_start;
		std::optional<size_t> _element_count;
	};
}
