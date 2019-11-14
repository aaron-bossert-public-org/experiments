
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/sync/vulkan_job_dependencies.h"
namespace igpu
{
	class vulkan_parameters;
	class vulkan_primitives;

	class vulkan_job_primitives : public vulkan_job_dependencies
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			VkPipelineLayout pipeline_layout = nullptr;
			vulkan_job* job = nullptr;
			size_t swap_count = 0;
			uint32_t descriptor_index = UINT32_MAX;
			const vulkan_parameters* parameters = nullptr;
			const vulkan_primitives* primitives = nullptr;
		};

		~vulkan_job_primitives();

		static std::shared_ptr< vulkan_job_primitives > make( const config& );

		const config& cfg() const;

	protected:
		vulkan_job_primitives( const config& );

		vulkan_job& job() override;

		const vulkan_job& job() const override;

		state& job_dependency_state() override;

		const state& job_dependency_state() const override;

		void on_record_cmds( VkCommandBuffer ) override;

		void on_gpu_object_reallocated( vulkan_dependency* ) override;

	private:
		const config _cfg;
		state _state;

		VkDescriptorPool _descriptor_pool = nullptr;
		std::vector< VkDescriptorSet > _descriptor_sets;
		std::vector< vulkan_parameter::config > _read_parameter_cfgs;
		std::vector< vulkan_parameter::config > _write_parameter_cfgs;

		bool _descriptors_dirty = false;
		size_t _swap_index = 0;
	};
}
