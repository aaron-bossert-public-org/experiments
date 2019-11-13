
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_job_dependencies.h"

namespace igpu
{
	class vulkan_geometry;
	class vulkan_graphics_pipeline;

	class vulkan_job_attributes : public vulkan_job_dependencies
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			vulkan_job* job = nullptr;
			size_t swap_count = 0;
			std::vector< size_t > vertex_buffer_indices;
			vulkan_geometry* geometry = nullptr;
		};

		const config& cfg() const;

		void bind_buffer_cmds( VkCommandBuffer ) const;

		static std::shared_ptr< vulkan_job_attributes > make( const config& );

		~vulkan_job_attributes();

		void on_reallocate_gpu_object( vulkan_dependency* ) override;

	protected:
		vulkan_job_attributes( const config& );

		const vulkan_job& vulkan_job_attributes::job() const override;

		state& job_dependency_state() override;

		const state& job_dependency_state() const override;

	private:
		const config _cfg;
		state _state;

		VkBuffer _index_buffer = nullptr;
		VkDeviceSize _index_buffer_offset = (VkDeviceSize)-1;
		VkIndexType _index_type = VK_INDEX_TYPE_MAX_ENUM;

		std::vector< VkBuffer > _vertex_buffers;
		std::vector< VkDeviceSize > _vertex_buffer_offsets;
	};
}
