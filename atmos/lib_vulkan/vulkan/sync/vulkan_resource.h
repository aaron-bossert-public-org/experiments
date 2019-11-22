
#pragma once

#include "vulkan/manager/vulkan_barrier_manager.h"
#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/sync/vulkan_job_scope.h"

#include "framework/utility/scoped_ptr.h"

#include <list>

namespace igpu
{
	class vulkan_dependency;
	class vulkan_queue;

	class vulkan_resource
	{
	public:
		using list = std::list< vulkan_dependency* >;
		using link = list::iterator;


		link add_dependency( vulkan_dependency* );

		void remove_dependency( const link& );


		const vulkan_barrier_manager::record_ref& barrier_record_ref() const;

		void barrier_record_ref( const vulkan_barrier_manager::record_ref& );


		ptrdiff_t write_count() const;

		void wait_pending_jobs() const;

		void on_barrier(
			vulkan_barrier_manager*,
			const scoped_ptr< vulkan_queue >&,
			VkImageLayout,
			const vulkan_job_scope& );

		bool validate_hazard(
			const vulkan_dependency* read_dependency,
			const std::vector< vulkan_dependency* >& read_hazards ) const;

		bool validate_barrier( VkImageLayout, const vulkan_job_scope& ) const;

		virtual bool is_valid_layout( VkImageLayout ) const = 0;

		virtual void update_descriptor_set(
			VkDescriptorSet descriptor_set,
			const vulkan_parameter::config&,
			size_t array_element ) const = 0;

		virtual ~vulkan_resource() = default;

	protected:
		class state
		{
			friend class vulkan_resource;
			scoped_ptr< vulkan_queue > queue;

			VkImageLayout layout = VK_IMAGE_LAYOUT_MAX_ENUM;

			list read_deps;
			list write_deps;

			size_t write_count = 0;

			vulkan_job_scope last_write_scope;
			vulkan_job_scope combined_read_scope;

			vulkan_barrier_manager::record_ref barrier_record_ref;
		};

		virtual vulkan_resource::state& resource_state() = 0;

		virtual const vulkan_resource::state& resource_state() const = 0;

		const scoped_ptr< vulkan_queue >& pending_queue() const;

		void reinitialized(
			const scoped_ptr< vulkan_queue >&,
			VkImageLayout layout,
			const vulkan_job_scope& );

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
