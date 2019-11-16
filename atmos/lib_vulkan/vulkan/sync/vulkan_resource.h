
#pragma once

#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/sync/vulkan_barrier_manager.h"
#include "vulkan/sync/vulkan_gpu_object.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include "framework/utility/scoped_ptr.h"

#include <list>

namespace igpu
{
	class vulkan_dependency;
	class vulkan_queue;

	class vulkan_resource : public vulkan_gpu_object
	{
	public:
		using list = std::list< vulkan_dependency* >;
		using link = list::iterator;

		struct state
		{
		private:
			friend vulkan_resource;

			scoped_ptr< vulkan_queue > queue;
			VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;

			list read_deps;
			list write_deps;

			size_t write_count = 0;

			vulkan_job_scope last_write_scope;
			vulkan_job_scope combined_read_scope;

			const vulkan_barrier_manager::record* barrier_manager_record =
				nullptr;
		};


		link add_read_only_dependency( vulkan_dependency* );

		link add_writeable_dependency( vulkan_dependency* );


		void remove_read_only_dependency( const link& );

		void remove_writeable_dependency( const link& );


		const vulkan_barrier_manager::record* barrier_manager_record();

		void barrier_manager_record( const vulkan_barrier_manager::record* );

		ptrdiff_t write_count() const;

		void wait_pending_jobs() const;

		void on_barrier(
			vulkan_barrier_manager*,
			const scoped_ptr< vulkan_queue >& );

		bool validate_barrier( VkImageLayout, const vulkan_job_scope& ) const;

		virtual vulkan_resource::state& resource_state() = 0;

		virtual const vulkan_resource::state& resource_state() const = 0;

		virtual void update_descriptor_set(
			VkDescriptorSet descriptor_set,
			const vulkan_parameter::config&,
			size_t array_element ) const = 0;

		virtual ~vulkan_resource() = 0;

	protected:
		void reinitialize(
			const scoped_ptr< vulkan_queue >&,
			const vulkan_job_scope&,
			VkImageLayout layout );

		virtual void push_barrier(
			uint32_t target_queue_family_index,
			vulkan_barrier_manager*,
			uint32_t src_queue_family_index,
			uint32_t dst_queue_family_index,
			VkImageLayout src_layout,
			VkImageLayout dst_layout,
			const vulkan_job_scope& src_scope,
			const vulkan_job_scope& dst_scope ) const = 0;
	};
}
