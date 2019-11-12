
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include <vector>

namespace igpu
{
	class vulkan_dependency;
	class vulkan_resource;

	class vulkan_barrier_manager
	{
	public:
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


		void begin_dependencies();

		void process_dependency( vulkan_dependency* );

		void end_dependencies( const scoped_ptr< vulkan_queue >& );

		void push_barrier(
			VkPipelineStageFlags src_stages,
			VkPipelineStageFlags dst_stages,
			const VkImageMemoryBarrier& );

		void push_barrier(
			VkPipelineStageFlags src_stages,
			VkPipelineStageFlags dst_stages,
			const VkBufferMemoryBarrier& );

		const pipeline_barrier& barrier() const;

	private:
		record* resolve( const record* );

	private:
		std::vector< record > _pending_records;
		pipeline_barrier _barrier;
	};
}
