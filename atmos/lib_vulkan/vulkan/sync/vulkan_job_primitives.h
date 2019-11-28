
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/sync/vulkan_job_dependencies.h"

#include <memory>

namespace igpu
{
	class vulkan_descriptor_pool;
	class vulkan_parameters;
	class vulkan_primitives;
	class vulkan_queue;

	class vulkan_job_primitives : public vulkan_job_dependencies
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_queue > queue;
			VkPipelineLayout pipeline_layout = nullptr;
			vulkan_job* job = nullptr;
			size_t swap_count = 0;
			uint32_t descriptor_index = UINT32_MAX;
			const vulkan_parameters* parameters = nullptr;
			const vulkan_primitives* primitives = nullptr;
		};

		~vulkan_job_primitives();

		static std::unique_ptr< vulkan_job_primitives > make( const config& );

		const config& cfg() const;

		void record_cmds( const scoped_ptr< vulkan_command_buffer >& ) override;

	protected:
		vulkan_job_primitives( const config& );

		vulkan_job& job() override;

		const vulkan_job& job() const override;

		state& job_dependency_state() override;

		const state& job_dependency_state() const override;


		void on_resource_reinitialized( vulkan_dependency* ) override;

	private:
		const config _cfg;
		state _state;

		std::shared_ptr< vulkan_descriptor_pool > _descriptor_pool;
		std::vector< VkDescriptorSet > _descriptor_sets;
		std::vector< vulkan_parameter::config > _read_parameter_cfgs;
		std::vector< vulkan_parameter::config > _write_parameter_cfgs;

		bool _descriptors_dirty = false;
		size_t _swap_index = 0;
	};
}
