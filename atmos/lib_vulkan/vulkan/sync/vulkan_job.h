
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_barrier_manager;
	class vulkan_job_dependencies;
	class vulkan_poset_fence;
	class vulkan_queue;

	class vulkan_job
	{
	public:
		class state
		{
			friend class vulkan_job;

			bool recording = false;
			ptrdiff_t fence_submit_index = 0;
			scoped_ptr< vulkan_poset_fence > fence;
			std::vector< vulkan_job_dependencies* > recorded_dependencies;
		};

		void start_recording_barriers();

		void record_barriers( vulkan_job_dependencies* );

		void submit_recorded_barriers(
			const scoped_ptr< vulkan_queue >&,
			vulkan_barrier_manager* );

		void wait_on_fence();

	protected:
		void fence( const scoped_ptr< vulkan_poset_fence >& );

		virtual state& job_state() = 0;

		virtual const state& job_state() const = 0;
	};
}