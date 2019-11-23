
#pragma once
#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <vector>

namespace igpu
{
	class frame_job_barrier;
	class vulkan_barrier_manager;
	class vulkan_queue_manager;
	class vulkan_resource;

	class vulkan_staging_manager
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_barrier_manager > barrier_manager;
			scoped_ptr< vulkan_queue_manager > queue_manager;
		};

		struct record_ref
		{
			const vulkan_staging_manager* recording_manager = nullptr;
			size_t record_index = SIZE_MAX;
		};

		void start_recording_transfers();

		void record_transfer( vulkan_resource* resource );

		void push_pending_transfers();

		void push_transfer(
			const std::initializer_list< frame_job_barrier >&,
			const std::function< void( VkCommandBuffer ) >& );

		static std::unique_ptr< vulkan_staging_manager > make( const config& );

		~vulkan_staging_manager();

	private:
		vulkan_staging_manager( const config& );

		vulkan_resource* resolve( const record_ref& );

	private:
		const config _cfg;
		bool _recording_transfers = false;
		VkCommandBuffer _vk_cmds = nullptr;
		std::vector< vulkan_resource* > _pending_resources;
		std::vector< frame_job_barrier > _pending_barriers;
	};
}
