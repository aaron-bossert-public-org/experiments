
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_job;
	class vulkan_dependency;
	class vulkan_barrier_manager;

	class vulkan_job_dependencies
	{
	public:
		struct state
		{
			std::vector< vulkan_dependency > read_deps;
			std::vector< vulkan_dependency > write_deps;
			std::vector< vulkan_dependency* > read_hazards;
		};

		void activate_read_hazard( vulkan_dependency* );

		void record_dependencies( vulkan_barrier_manager* );

		bool validate_barriers() const;

		void wait_pending_job() const;

		void record_cmds( VkCommandBuffer );

		void gpu_object_reallocated( vulkan_dependency* );

		virtual ~vulkan_job_dependencies();

	protected:
		virtual vulkan_job& job() = 0;

		virtual const vulkan_job& job() const = 0;

		virtual state& job_dependency_state() = 0;

		virtual const state& job_dependency_state() const = 0;

		virtual void on_record_cmds( VkCommandBuffer ) = 0;

		virtual void on_gpu_object_reallocated( vulkan_dependency* ) = 0;
	};
}