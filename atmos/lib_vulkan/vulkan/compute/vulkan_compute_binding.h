
#pragma once

#include "vulkan/sync/vulkan_job.h"

#include "igpu/compute/compute_binding.h"

#include "framework/utility/scoped_ptr.h"

#include <memory>

namespace igpu
{
	class vulkan_command_buffer;
	class vulkan_command_pool;
	class vulkan_compute_pipeline;
	class vulkan_compute_program;
	class vulkan_job_primitives;
	class vulkan_managers;
	class vulkan_primitives;

	class vulkan_compute_binding
		: public compute_binding
		, public vulkan_job
	{
	public:
		struct config : compute_binding::config
		{
			struct vulkan
			{
				scoped_ptr< vulkan_managers > managers;
				std::shared_ptr< vulkan_compute_program > program;
				std::shared_ptr< vulkan_compute_pipeline > pipeline;
				std::shared_ptr< vulkan_primitives > batch;
				std::shared_ptr< vulkan_primitives > material;
				std::shared_ptr< vulkan_primitives > instance;
				size_t swap_count = 0;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		void dispatch() override;

		void compute_params( const compute_parameters& ) override;

		const compute_parameters& compute_params() const override;

		state& job_state() override;

		const state& job_state() const override;

		static std::unique_ptr< vulkan_compute_binding > make( const config& );

		~vulkan_compute_binding();

	private:
		vulkan_compute_binding( const config& );

		struct swap_state
		{
			vulkan_poset_fence fence;
			std::shared_ptr< vulkan_command_buffer > cmds;
		};

		struct state
		{
			std::shared_ptr< vulkan_job_primitives > batch_job_primitives;
			std::shared_ptr< vulkan_job_primitives > material_job_primitives;
			std::shared_ptr< vulkan_job_primitives > instance_job_primitives;
			std::shared_ptr< vulkan_command_pool > command_pool;
			std::vector< swap_state > swap_states;
			compute_parameters compute_params;
			size_t swap_index = 0;
		};

		swap_state& current_swap_state();

		const config _cfg;
		state _st;
		vulkan_job::state _job_state;
	};
}
