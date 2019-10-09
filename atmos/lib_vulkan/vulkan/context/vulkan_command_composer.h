
#pragma once

#include <memory>
#include <vector>
#include <vulkan/resource/vulkan_meta_queue.h>

struct VmaAllocator_T;

namespace igpu
{
	class vulkan_command_composer
	{
	public:

		struct config
		{
			bool enable_validation_layers = false;
			size_t max_inflight_frames = 3;
		};

		const config& cfg() const;

		~vulkan_command_composer();

		static std::unique_ptr<vulkan_command_composer> make(const config&);

	protected:

		vulkan_command_composer(
			const config&);

	private:

		const config _cfg;

		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT _debug_messenger;
		VmaAllocator_T* _vma;

		VkDescriptorPool _descriptor_pool;

		vulkan_meta_queue _present_queue;
		vulkan_meta_queue _graphics_queue;
		vulkan_meta_queue _compute_queue;
		vulkan_meta_queue _transfer_queue;
	};
}
