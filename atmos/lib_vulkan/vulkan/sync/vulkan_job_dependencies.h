
#pragma once

#include "vulkan/sync/vulkan_dependency.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_barrier_manager;
	class vulkan_dependency;
	class vulkan_parameters;
	class vulkan_primitives;

	class vulkan_job_dependencies
		: public std::enable_shared_from_this< vulkan_job_dependencies >
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			size_t swap_count = 0;
			const vulkan_parameters* parameters = nullptr;
			const vulkan_primitives* primitives = nullptr;
		};

		void activate_read_hazard( vulkan_dependency* );

		void process_dependencies( vulkan_barrier_manager* );

		void on_reallocate_gpu_object( vulkan_dependency* );

		VkDescriptorSet descriptor_set();

		static std::shared_ptr< vulkan_job_dependencies > make( const config& );

	private:
		// prevent calling code from being able to costruct
		// vulkan_job_dependencies (must call make instead)
		struct this_is_private
		{
			const config& cfg;
		};

	public:
		vulkan_job_dependencies( const this_is_private& );

		~vulkan_job_dependencies();

	private:
		const config _cfg;

		VkDescriptorPool _descriptor_pool;
		std::vector< VkDescriptorSet > _descriptor_sets;
		std::vector< vulkan_dependency > _read_deps;
		std::vector< vulkan_dependency > _write_deps;

		bool _descriptors_dirty = false;
		size_t _swap_index = 0;
		std::vector< vulkan_dependency* > _read_hazards;
	};
}
