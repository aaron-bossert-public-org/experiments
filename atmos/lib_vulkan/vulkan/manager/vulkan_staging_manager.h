
#pragma once

#include "vulkan/manager/vulkan_barrier_manager.h"

#include "framework/utility/scoped_ptr.h"

#include <vector>

namespace igpu
{
	class vulkan_resource;
	class vulkan_barrier_manager;
	class vulkan_queue_manager;

	class vulkan_staging_manager
	{
	public:
		// struct buffer_transfer
		//{
		//	VkBufferCopy region = {};
		//	vulkan_buffer* src = nullptr;
		//	vulkan_buffer* dst = nullptr;
		//};

		// struct image_transfer
		//{
		//	VkBufferCopy region = {};
		//	vulkan_buffer* src = nullptr;
		//	vulkan_buffer* dst = nullptr;
		//};

		struct config
		{
			VkDevice device = nullptr;
			scoped_ptr< vulkan_queue_manager > queue_manager;
			scoped_ptr< vulkan_barrier_manager > barrier_manager;
		};

		struct record_ref
		{
			const vulkan_staging_manager* recording_manager = nullptr;
			size_t record_index = SIZE_MAX;
		};

		void start_recording_transfers();

		void record_transfer( vulkan_resource* resource );

		void push_transfers();

		// void push_transfer_barrier( const frame_job_barrier& );

		// void push_transfer( const image_transfer& );

		static std::unique_ptr< vulkan_staging_manager > make( const config& );

		~vulkan_staging_manager();

	private:
		struct transfer_semaphores
		{
			std::unique_ptr< vulkan_semaphore > wait;
			std::unique_ptr< vulkan_semaphore > signal;
		};

		vulkan_staging_manager( const config& );

		vulkan_resource* resolve( const record_ref& );

	private:
		const config _cfg;
		bool _recording_transfers = false;
		std::vector< vulkan_resource* > _pending_resources;
	};
}
