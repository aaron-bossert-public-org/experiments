
#pragma once

#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/buffer/vulkan_vertex_buffer.h"
#include "vulkan/defines/vulkan_includes.h"

#include "igpu/buffer/geometry.h"

#include <memory>

namespace igpu
{
	VkPrimitiveTopology to_vulkan_topology( topology topology );

	class vulkan_poset_fence;

	class vulkan_geometry : public geometry
	{
	public:
		struct config : geometry::config
		{
			struct vulkan
			{
				VkPrimitiveTopology topology;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		igpu::draw_parameters draw_parameters() const override;

		vulkan_index_buffer& index_buffer() override;

		const vulkan_index_buffer& index_buffer() const override;

		size_t vertex_buffer_count() const override;

		vulkan_vertex_buffer& vertex_buffer( size_t index ) override;

		const vulkan_vertex_buffer& vertex_buffer(
			size_t index ) const override;

		static std::unique_ptr< vulkan_geometry > make( const config& );

		~vulkan_geometry() override;

	protected:
		vulkan_geometry(
			const config&,
			igpu::vulkan_index_buffer* const,
			std::vector< vulkan_vertex_buffer* > );

	private:
		const config _cfg;
		ptrdiff_t _base_vertex = 0;
		size_t _instance_start = 0;
		size_t _instance_count = 1;
		size_t _element_start = 0;
		std::optional< size_t > _element_count;
		igpu::vulkan_index_buffer* const _index_buffer = nullptr;
		const std::vector< vulkan_vertex_buffer* > _vertex_buffers;
	};
}
