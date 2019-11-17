
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
	class vulkan_fence;
	class vulkan_image;
	class vulkan_queue;
	class vulkan_resource;
	class vulkan_semaphore;
	class vulkan_synchronization;

	class vulkan_barrier_manager
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			scoped_ptr< vulkan_synchronization > synchronization;
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
			vulkan_resource* resource;
			VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
			vulkan_job_scope job_scope;
		};

		void submit_frame_job(
			const scoped_ptr< vulkan_queue >,
			const std::initializer_list< frame_job_barrier >&,
			const std::function< void( VkCommandBuffer ) >& );

		void start_recording_barriers();

		void record_barrier(
			vulkan_resource* resource,
			VkImageLayout layout,
			const vulkan_job_scope& job_scope );

		void submit_recorded_barriers( const scoped_ptr< vulkan_queue >& );

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

		record* resolve( const record* );

		pipeline_barrier& barrier( uint32_t queue_family_index );

		const pipeline_barrier& barrier( uint32_t queue_family_index ) const;

		transfer_semaphores& transfer_semaphores_for_queues(
			size_t src_compact_queue_index,
			size_t dst_compact_queue_index );

	private:
		const config _cfg;
		bool _recording_barriers = false;
		std::vector< record > _pending_records;
		std::vector< pipeline_barrier > _barriers;
		std::vector< std::vector< transfer_semaphores > > _transfer_semaphores;
	};

	class frame_job_barrier
	{
	public:
		frame_job_barrier(
			vulkan_buffer* buffer,
			const vulkan_job_scope& scope );

		frame_job_barrier(
			vulkan_image* image,
			VkImageLayout layout_,
			const vulkan_job_scope& scope );

	private:
		friend class vulkan_barrier_manager;

		vulkan_resource* resource = nullptr;
		VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;
		vulkan_job_scope job_scope;
	};
}
