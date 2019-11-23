
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class frame_job_barrier;

	class vulkan_buffer;
	class vulkan_barrier_manager;
	class vulkan_image;
	class vulkan_queue;
	class vulkan_queue_manager;
	class vulkan_resource;
	class vulkan_staging_manager;

	class vulkan_managers
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_barrier_manager > barrier;
			scoped_ptr< vulkan_queue_manager > queues;
			scoped_ptr< vulkan_staging_manager > staging;
		};

		const config& cfg() const;

		void push_frame_job(
			const scoped_ptr< vulkan_queue >,
			const std::initializer_list< frame_job_barrier >&,
			const std::function< void( VkCommandBuffer ) >& );

		~vulkan_managers();

		static std::unique_ptr< vulkan_managers > make( const config& );

	protected:
		vulkan_managers( const config& );

	private:
		const config _cfg;
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

		vulkan_resource* resource() const;

		VkImageLayout layout() const;

		const vulkan_job_scope& job_scope() const;

	private:
		vulkan_resource* _resource = nullptr;
		VkImageLayout _layout = VK_IMAGE_LAYOUT_MAX_ENUM;
		vulkan_job_scope _job_scope;
	};
}
