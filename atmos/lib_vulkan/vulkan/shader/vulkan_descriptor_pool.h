
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

#include <array>

namespace igpu
{
	class vulkan_parameters;
	class vulkan_queue;

	class vulkan_descriptor_pool
	{
	public:
		struct config
		{
			scoped_ptr< vulkan_queue > queue;
			VkDescriptorPoolCreateFlags flags;
			VkDescriptorSetLayout layout = nullptr;
			uint32_t max_sets = 0;
			uint32_t size_count = 0;
			std::array< VkDescriptorPoolSize, 3 > sizes;
		};

		const config& cfg() const;

		VkDescriptorSet allocate_descriptor_set();

		static std::unique_ptr< vulkan_descriptor_pool > make(
			const scoped_ptr< vulkan_queue >&,
			VkDescriptorPoolCreateFlags,
			const vulkan_parameters&,
			uint32_t max_sets );

		static std::unique_ptr< vulkan_descriptor_pool > make( const config& );

		~vulkan_descriptor_pool();

	private:
		friend class vulkan_queue;

		vulkan_descriptor_pool( const config&, VkDevice, VkDescriptorPool );

	private:
		config _cfg;
		VkDevice _device = nullptr;
		VkDescriptorPool _vk_pool = nullptr;
	};
}
