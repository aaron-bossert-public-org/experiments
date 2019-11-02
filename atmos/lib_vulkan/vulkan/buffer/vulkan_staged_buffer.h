
#pragma once

#include <framework/utility/scoped_ptr.h>

#include <igpu/buffer/buffer_usage.h>

#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/defines/vulkan_includes.h>

namespace igpu
{
	class vulkan_synchronization;
	class vulkan_queue;

	class vulkan_staged_buffer
	{
	public:

		struct config
		{
			buffer_usage usage;
			VkBufferUsageFlagBits vk_usage_flags = (VkBufferUsageFlagBits)0;
			scoped_ptr < vulkan_synchronization > synchronization;
		};
		
		vulkan_staged_buffer(
			const config&);

		~vulkan_staged_buffer();

		const config& cfg() const;

		void map(
			size_t byte_size,
			buffer_view_base*);

		void unmap();

		size_t byte_capacity() const;
		
		void release();

		vulkan_buffer& gpu_buffer();

		const vulkan_buffer& gpu_buffer() const;

	private:
		
		const config _cfg;
		size_t _last_mapped_bytes = 0;
		vulkan_buffer _staging_buffer;
		vulkan_buffer _gpu_buffer;
	};

	template <typename T>
	class vulkan_staged_buffer_t : public T
	{
	public:

		using config = typename T::config;

		~vulkan_staged_buffer_t() override {};

		const config& cfg() const override
		{
			return _cfg;
		}

		void map(
			size_t byte_size,
			buffer_view_base* out_buffer_view)
			override
		{
			_vulkan_staged_buffer.map(byte_size, out_buffer_view);
		}

		void unmap() override
		{
			_vulkan_staged_buffer.unmap();
		}

		size_t byte_capacity() const override
		{
			return _vulkan_staged_buffer.byte_capacity();
		}

		vulkan_buffer& gpu_resource() override
		{
			return _vulkan_staged_buffer.gpu_buffer();
		}

		const vulkan_buffer& gpu_resource() const override
		{
			return _vulkan_staged_buffer.gpu_buffer();
		}

		template<typename... ARGS>
		static std::unique_ptr<vulkan_staged_buffer_t> make(
			const config& cfg,
			const vulkan_staged_buffer::config& res_cfg,
			const ARGS& ... args)
		{
			if (!is_valid(cfg.usage))
			{
				LOG_CRITICAL("invalid usage:%d", (int)cfg.usage);
			}
			else if (!is_valid(res_cfg.usage))
			{
				LOG_CRITICAL("invalid res_cfg usage:%d", (int)cfg.usage);
			}
			else if (0 == res_cfg.vk_usage_flags)
			{
				LOG_CRITICAL("vk_usage_flags is 0");
			}
			else if (!res_cfg.synchronization)
			{
				LOG_CRITICAL("buffer mediator has expired");
			}
			else
			{
				return std::unique_ptr<vulkan_staged_buffer_t>(
					new vulkan_staged_buffer_t(
						cfg,
						res_cfg,
						args...));
			}

			return nullptr;
		}

	private:

		template<typename... ARGS>
		vulkan_staged_buffer_t(
			const config& cfg,
			const vulkan_staged_buffer::config& res_config,
			const ARGS& ... args)
			: T(args...)
			, _cfg(cfg)
			, _vulkan_staged_buffer(res_config)
		{
		}

	private:

		const config _cfg;
		vulkan_staged_buffer _vulkan_staged_buffer;
	};
}
