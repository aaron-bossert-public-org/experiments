
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_barrier_manager;
	class vulkan_fence;
	class vulkan_job_dependencies;
	class vulkan_queue;

	class vulkan_job
	{
	public:
		class state
		{
			friend class vulkan_job;

			std::shared_ptr< vulkan_fence > fence;
			std::vector< vulkan_job_dependencies* > recorded_dependencies;
		};

		void begin_record_cmds();

		void on_record_cmds( vulkan_job_dependencies* );

		void barrier_recorded_commands(
			const scoped_ptr< vulkan_queue >&,
			vulkan_barrier_manager* );

		void wait_on_fence() const;

	protected:
		void fence( const std::shared_ptr< vulkan_fence >& );

		virtual state& job_state() = 0;

		virtual const state& job_state() const = 0;
	};
}