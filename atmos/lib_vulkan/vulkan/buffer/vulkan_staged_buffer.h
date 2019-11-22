
#pragma once

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_barrier_manager;
	class vulkan_queue_manager;
	class vulkan_queue;

	class vulkan_staged_buffer
	{
	public:
		struct config
		{
			VkDevice device = nullptr;
			const VkPhysicalDeviceProperties* device_properties = nullptr;
			VmaAllocator vma = nullptr;
			VkBufferUsageFlagBits vk_usage_flags = (VkBufferUsageFlagBits)0;
			scoped_ptr< vulkan_queue_manager > queue_manager;
			scoped_ptr< vulkan_barrier_manager > barrier_manager;
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
			const vulkan_staged_buffer::config& cfg,
			const ARGS&... args )
		{
			if ( !is_valid( cfg.memory ) )
			{
				LOG_CRITICAL( "invalid memory:%d", (int)cfg.memory );
			}
			else if ( 0 == cfg.vk_usage_flags )
			{
				LOG_CRITICAL( "vk_usage_flags is 0" );
			}
			else if ( !cfg.queue_manager )
			{
				LOG_CRITICAL( "vulkan queue manager object has expired" );
			}
			else
			{
				return std::unique_ptr< vulkan_staged_buffer_t >(
					new vulkan_staged_buffer_t( cfg, args... ) );
			}

			return nullptr;
		}

	private:
		template < typename... ARGS >
		vulkan_staged_buffer_t(
			const vulkan_staged_buffer::config& cfg,
			const ARGS&... args )
			: T( args... )
			, _vulkan_staged_buffer( cfg )
		{}

	private:
		vulkan_staged_buffer _vulkan_staged_buffer;
	};
}
