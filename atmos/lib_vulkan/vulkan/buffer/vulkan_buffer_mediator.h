
#pragma once

#include <memory>
#include <vector>
#include <igpu/buffer/compute_buffer.h>
#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/vertex_buffer.h>

struct VmaAllocator_T;

namespace igpu
{
	class vulkan_buffer;
	class vulkan_queue;

	class vulkan_buffer_mediator
	{
	public:

		struct config
		{
			std::shared_ptr<vulkan_queue> present_queue;
			std::shared_ptr<vulkan_queue> graphics_queue;
			std::shared_ptr<vulkan_queue> compute_queue;
			std::shared_ptr<vulkan_queue> transfer_queue;
			size_t max_inflight_frames = 3;
		};

		const config& cfg() const;

		VmaAllocator vma();

		void copy(
			const std::shared_ptr < vulkan_buffer >& src,
			const std::shared_ptr < vulkan_buffer >& dst);

		~vulkan_buffer_mediator();

		static std::unique_ptr<vulkan_buffer_mediator> make(const config&);

	protected:

		vulkan_buffer_mediator(
			const config&);

	private:

		const config _cfg;

		
		VkDebugUtilsMessengerEXT _debug_messenger;
		VmaAllocator_T* _vma;

		VkDescriptorPool _descriptor_pool;
	};
}
