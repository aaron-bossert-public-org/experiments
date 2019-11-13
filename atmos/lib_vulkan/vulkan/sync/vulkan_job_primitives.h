
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
			vulkan_job* job = nullptr;
			size_t swap_count = 0;
			const vulkan_parameters* parameters = nullptr;
			const vulkan_primitives* primitives = nullptr;
		};

		VkDescriptorSet descriptor_set();

		~vulkan_job_primitives();

		static std::shared_ptr< vulkan_job_primitives > make( const config& );

		const config& cfg() const;

		void on_reallocate_gpu_object( vulkan_dependency* ) override;

	protected:
		vulkan_job_primitives( const config& );

		const vulkan_job& job() const override;

		state& job_dependency_state() override;

		const state& job_dependency_state() const override;

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
