
#pragma once

#include <vulkan/buffer/vulkan_index_buffer.h>
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <vulkan/defines/vulkan_includes.h>

#include <igpu/buffer/geometry.h>

#include <optional>

namespace igpu
{
	VkPrimitiveTopology to_vulkan_topology(topology topology);
	class vulkan_fence;
	
	class vulkan_geometry : public geometry
	{
	public:

		struct config : geometry::config
		{
			struct vulkan
			{
				VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {};
			};

			vulkan vk;
		};

		const config& cfg() const override;

		const vulkan_index_buffer& index_buffer() const override;
		
		size_t vertex_buffer_count() const override;

		const vulkan_vertex_buffer& vertex_buffer(size_t index) const override;

		static std::unique_ptr<vulkan_geometry> make(
			const config&);

		void add_fence(const std::shared_ptr<vulkan_fence>&);

		~vulkan_geometry() override;

	protected:

		vulkan_geometry(
			const config&,
			igpu::vulkan_index_buffer* const,
			std::vector < vulkan_vertex_buffer* >);

	private:

		const config _cfg;
		igpu::vulkan_index_buffer* const _index_buffer = nullptr;
		const std::vector < vulkan_vertex_buffer* > _vertex_buffers;
	};
}
