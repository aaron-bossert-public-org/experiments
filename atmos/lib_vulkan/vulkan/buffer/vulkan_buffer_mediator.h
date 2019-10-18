
#pragma once

#include <memory>
#include <vector>
#include <igpu/buffer/compute_buffer.h>
#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/vertex_buffer.h>

struct VkPhysicalDevice_T;
struct VkDevice_T;
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
			VkPhysicalDevice_T* physical_device;
			VkDevice_T* device;
			std::shared_ptr<vulkan_queue> present_queue;
			std::shared_ptr<vulkan_queue> graphics_queue;
			std::shared_ptr<vulkan_queue> compute_queue;
			std::shared_ptr<vulkan_queue> transfer_queue;
		};

		const config& cfg() const;

		VmaAllocator_T* vma();

		void copy(
			const std::shared_ptr < vulkan_buffer >& src,
			const std::shared_ptr < vulkan_buffer >& dst);

		~vulkan_buffer_mediator();

		static std::unique_ptr<vulkan_buffer_mediator> make(const config&);

	protected:

		vulkan_buffer_mediator(
			const config&,
			VmaAllocator_T*);

	private:

		const config _cfg;
		VmaAllocator_T* _vma;
	};
}
