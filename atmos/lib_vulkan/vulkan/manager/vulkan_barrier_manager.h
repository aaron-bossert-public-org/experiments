
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include "framework/utility/scoped_ptr.h"

#include <initializer_list>
#include <vector>

namespace igpu
{
	class frame_job_barrier;

	class vulkan_buffer;
	class vulkan_dependency;
	class vulkan_image;
	class vulkan_poset_fence;
	class vulkan_queue;
	class vulkan_resource;
	class vulkan_semaphore;
	class vulkan_queue_manager;

	class vulkan_barrier_manager
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			scoped_ptr< vulkan_queue_manager > queue_manager;
		};

		struct pipeline_barrier
		{
			VkPipelineStageFlags src_stages = 0;
			VkPipelineStageFlags dst_stages = 0;
			VkPipelineStageFlags transfer_stages = 0;
			std::vector< VkBufferMemoryBarrier > buffer_memory_barriers;
			std::vector< VkImageMemoryBarrier > image_memory_barriers;
		};

		struct record
		{
			vulkan_resource* resource = nullptr;
			VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
			vulkan_job_scope job_scope;
		};

		struct record_ref
		{
			const vulkan_barrier_manager* recording_manager = nullptr;
			size_t record_index = SIZE_MAX;
		};

		void push_frame_job(
			const scoped_ptr< vulkan_queue >,
			const std::initializer_list< frame_job_barrier >&,
			const std::function< void( VkCommandBuffer ) >& );


		void start_recording_barriers();

		void record_barrier(
			vulkan_resource* resource,
			VkImageLayout layout,
			const vulkan_job_scope& job_scope );

		void push_recorded_barriers( const scoped_ptr< vulkan_queue >& );

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

		~vulkan_barrier_manager();

	private:
		struct transfer_semaphores
		{
			std::unique_ptr< vulkan_semaphore > wait;
			std::unique_ptr< vulkan_semaphore > signal;
		};

		vulkan_barrier_manager(
			const config&,
			std::vector< std::vector< transfer_semaphores > >&& );

		record* resolve( const record_ref& );

	private:
		const config _cfg;
		bool _recording_barriers = false;
		std::vector< record > _pending_records;
		std::vector< pipeline_barrier > _barrier_family_table;
		std::vector< std::vector< transfer_semaphores > >
			_semaphore_family_tables;
	};

	class frame_job_barrier
	{
	public:
		frame_job_barrier(
			vulkan_buffer* buffer,
			decorator decorators,
			VkPipelineStageFlagBits stage,
			VkAccessFlagBits access );

		frame_job_barrier(
			vulkan_image* image,
			VkImageLayout layout_,
			decorator decorators,
			VkPipelineStageFlagBits stage,
			VkAccessFlagBits access );

	private:
		friend class vulkan_barrier_manager;

		vulkan_resource* resource = nullptr;
		VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
		vulkan_job_scope job_scope;
	};
}
