
#pragma once

#include <memory>
#include <vector>

namespace igpu
{
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

		void process_dependencies( vulkan_barrier_manager* );

		bool validate_barriers() const;

		virtual void on_reallocate_gpu_object( vulkan_dependency* ) = 0;

	protected:
		virtual state& job_dependency_state() = 0;

		virtual const state& job_dependency_state() const = 0;
	};
}
