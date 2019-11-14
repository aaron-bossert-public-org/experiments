
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include <vector>

namespace igpu
{
	class vulkan_dependency;
	class vulkan_resource;
	class vulkan_synchronization;

	class vulkan_barrier_manager
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_synchronization > synchronization;
		};

		struct pipeline_barrier
		{
			VkPipelineStageFlags src_stages = 0;
			VkPipelineStageFlags dst_stages = 0;
			std::vector< VkBufferMemoryBarrier > buffer_memory_barriers;
			std::vector< VkImageMemoryBarrier > image_memory_barriers;
		};

		struct record
		{
			vulkan_resource* resource;
			VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
			vulkan_job_scope job_scope;
		};

		void start_dependency_barriers();

		void record_dependency( vulkan_dependency* );

		void finish_dependency_barriers( const scoped_ptr< vulkan_queue >& );

		// invoked in indirectly via finish_dependency_barriers
		void push_barrier(
			uint32_t target_family_index,
			VkPipelineStageFlags src_stages,
			VkPipelineStageFlags dst_stages,
			const VkImageMemoryBarrier& );

		// invoked in indirectly via finish_dependency_barriers
		void push_barrier(
			uint32_t target_family_index,
			VkPipelineStageFlags src_stages,
			VkPipelineStageFlags dst_stages,
			const VkBufferMemoryBarrier& );

		static std::unique_ptr< vulkan_barrier_manager > make( const config& );

	private:
		vulkan_barrier_manager( const config& );

		record* resolve( const record* );

		pipeline_barrier& barrier( uint32_t queue_family_index );

		const pipeline_barrier& barrier( uint32_t queue_family_index ) const;

	private:
		const config _cfg;
		bool _recording_barriers = false;
		std::vector< record > _pending_records;
		std::vector< pipeline_barrier > _barriers;
	};
}
