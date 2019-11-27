
#pragma once

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_managers;
	class vulkan_queue;

	class vulkan_staged_buffer
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			VmaAllocator vma = nullptr;
			VkBufferUsageFlagBits vk_usage_flags = (VkBufferUsageFlagBits)0;
			scoped_ptr< vulkan_managers > managers;
			memory_type memory;
		};

		vulkan_staged_buffer( const config& );

		~vulkan_staged_buffer();

		void map( buffer_view_base* );

		void unmap();

		vulkan_buffer& gpu_buffer();

		const vulkan_buffer& gpu_buffer() const;

		size_t byte_size() const;

	private:
		const config _cfg;
		size_t _last_write_count = 0;
		size_t _byte_size = 0;
		vulkan_buffer _staging_buffer;
		vulkan_buffer _gpu_buffer;
	};

	template < typename T >
	class vulkan_staged_buffer_t : public T
	{
	public:
		using config = typename T::config;

		~vulkan_staged_buffer_t() override{};

		const config& cfg() const override
		{
			return _cfg;
		}

		void map( buffer_view_base* out_buffer_view ) override
		{
			_vulkan_staged_buffer.map( out_buffer_view );
		}

		void unmap() override
		{
			_vulkan_staged_buffer.unmap();
		}

		size_t byte_size() const
		{
			return _vulkan_staged_buffer.byte_size();
		}

		vulkan_buffer& gpu_object() override
		{
			return _vulkan_staged_buffer.gpu_buffer();
		}

		const vulkan_buffer& gpu_object() const override
		{
			return _vulkan_staged_buffer.gpu_buffer();
		}

		template < typename... ARGS >
		static std::unique_ptr< vulkan_staged_buffer_t > make(
			const config& cfg,
			VkBufferUsageFlagBits vk_usage_flags,
			const ARGS&... args )
		{
			vulkan_staged_buffer::config staged_cfg = {
				cfg.vk.device,
				cfg.vk.vma,
				vk_usage_flags,
				cfg.vk.managers,
				cfg.memory,
			};

			if ( !is_valid( staged_cfg.memory ) )
			{
				LOG_CRITICAL( "invalid memory:%d", (int)staged_cfg.memory );
			}
			else if ( 0 == staged_cfg.vk_usage_flags )
			{
				LOG_CRITICAL( "vk_usage_flags is 0" );
			}
			else if ( !staged_cfg.managers )
			{
				LOG_CRITICAL( "vulkan managers object has expired" );
			}
			else
			{
				return std::unique_ptr< vulkan_staged_buffer_t >(
					new vulkan_staged_buffer_t( cfg, staged_cfg, args... ) );
			}

			return nullptr;
		}

	private:
		template < typename... ARGS >
		vulkan_staged_buffer_t(
			const config& cfg,
			const vulkan_staged_buffer::config& staged_cfg,
			const ARGS&... args )
			: T( args... )
			, _cfg(cfg)
			, _vulkan_staged_buffer( staged_cfg )
		{}

	private:
		const config _cfg;
		vulkan_staged_buffer _vulkan_staged_buffer;
	};
}
