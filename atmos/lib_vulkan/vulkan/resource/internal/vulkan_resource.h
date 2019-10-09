
#pragma once

#include <framework/perf/metrics.h>

#include <igpu/resource/resource_usage.h>

#include <vulkan/resource/internal/vulkan_payload.h>

namespace igpu
{
	class vulkan_resource_mediator;
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

	template <typename T>
	class vulkan_resource_t : public T
	{
	public:

		using config = typename T::config;

		~vulkan_resource_t() override {};

		const config& cfg() const override
		{
			return _cfg;
		}

		void map(
			buffer_view_base& resource_view,
			size_t count) 
			override
		{
			_vulkan_resource.map(resource_view, count);
		}

		void map_subrange(
			buffer_view_base& resource_view,
			size_t offset,
			size_t count)
			override
		{
			_vulkan_resource.map_subrange(resource_view, offset, count);
		}

		void unmap(
			buffer_view_base& resource_view)
			override
		{
			_vulkan_resource.unmap(resource_view);
		}

		size_t capacity() const override
		{
			return _vulkan_resource.capacity();
		}

		static std::unique_ptr<vulkan_resource_t> vulkan_resource_t::make(
			const config& cfg, 
			const std::string_view& metric,
			const vulkan_resource::config& res_config)
		{
			if (!is_valid(cfg.usage))
			{
				LOG_CONTEXT(CRITICAL, "unhandled usage:%s", to_string(cfg.usage).data());
			}
			else
			{
				return std::unique_ptr<vulkan_vertex_resource>(new vulkan_vertex_resource(cfg));
			}

			return nullptr;
		}

	protected:

		vulkan_resource_t(
			const config& cfg,
			const std::string_view& metric,
			const vulkan_resource::config& res_config)
			: _cfg(cfg)
			, _vulkan_resource(
				metric,
				res_config)
		{
		}

	private:

		const config _cfg;
		vulkan_resource _vulkan_resource;
	};
}
