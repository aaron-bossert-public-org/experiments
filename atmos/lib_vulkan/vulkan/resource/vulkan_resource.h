
#pragma once

#include <framework/perf/metrics.h>

#include <igpu/resource/resource_usage.h>

#include <vulkan/resource/vulkan_payload.h>

namespace igpu
{
	class vulkan_command_composer;
	class vulkan_queue;

	class vulkan_resource
	{
	public:

		struct config
		{
			resource_usage usage;
			VkBufferUsageFlagBits vk_usage_flags = (VkBufferUsageFlagBits)0;
		};
		
		vulkan_resource(
			const std::string_view& metric,
			const config&);

		~vulkan_resource();

		const config& cfg() const;

		void map(
			buffer_view_base&,
			size_t count);

		void map_subrange(
			buffer_view_base&,
			size_t offset,
			size_t count);

		void unmap(
			buffer_view_base&);

		void unmap();

		void release();

		size_t capacity() const;

	private:

		const config _cfg;

		VkBuffer_T* _staging_buffer = nullptr;
		VmaAllocation_T* _staging_allocation = nullptr;
		
		void* _mapped;
		size_t _mapped_size;
		size_t _mapped_offset;

		vulkan_payload _cpu_payload;
		vulkan_payload _gpu_payload;

		perf::metric _gpu_mem_metric;
		perf::metric _cpu_mem_metric;
	};
}
