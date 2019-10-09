
#pragma once

#include <memory>
#include <vector>
#include <vulkan/resource/internal/vulkan_queue.h>
#include <igpu/resource/compute_resource.h>
#include <igpu/resource/index_resource.h>
#include <igpu/resource/vertex_resource.h>

struct VmaAllocator_T;

namespace igpu
{
	class vulkan_resource_mediator
	{
	public:

		struct config
		{
			bool enable_validation_layers = false;
			size_t max_inflight_frames = 3;
		};

		const config& cfg() const;

		~vulkan_resource_mediator();

		static std::unique_ptr<vulkan_resource_mediator> make(const config&);

		std::unique_ptr<compute_resource> make_resource(const compute_resource::config&);

		std::unique_ptr<index_resource> make_resource(const index_resource::config&);
		
		std::unique_ptr<vertex_resource> make_resource(const vertex_resource::config&);

	protected:

		vulkan_resource_mediator(
			const config&,
			std::unique_ptr<vulkan_queue> present_queue,
			std::unique_ptr<vulkan_queue> graphics_queue,
			std::unique_ptr<vulkan_queue> compute_queue,
			std::unique_ptr<vulkan_queue> transfer_queue);

	private:

		const config _cfg;

		VkInstance _instance = VK_NULL_HANDLE;
		VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
		VkDevice _device = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT _debug_messenger;
		VmaAllocator_T* _vma;

		VkDescriptorPool _descriptor_pool;

		std::unique_ptr<vulkan_queue> _present_queue;
		std::unique_ptr<vulkan_queue> _graphics_queue;
		std::unique_ptr<vulkan_queue> _compute_queue;
		std::unique_ptr<vulkan_queue> _transfer_queue;
	};
}
