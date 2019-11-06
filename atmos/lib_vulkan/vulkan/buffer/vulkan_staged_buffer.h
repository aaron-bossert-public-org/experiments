
#pragma once

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/defines/vulkan_includes.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_synchronization;
	class vulkan_queue;

	class vulkan_staged_buffer
	{
	public:
		struct config
		{
			VkBufferUsageFlagBits vk_usage_flags = (VkBufferUsageFlagBits)0;
			scoped_ptr< vulkan_synchronization > synchronization;
			buffer_mapping mapping;
		};

		vulkan_staged_buffer( const config& );

		~vulkan_staged_buffer();

		void map( buffer_view_base* );

		void unmap();

		void release();

		vulkan_buffer& gpu_buffer();

		const vulkan_buffer& gpu_buffer() const;

		size_t byte_size() const;

	private:
		const config _cfg;
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
			if ( !is_valid( cfg.mapping ) )
			{
				LOG_CRITICAL( "invalid mapping:%d", (int)cfg.mapping );
			}
			else if ( 0 == cfg.vk_usage_flags )
			{
				LOG_CRITICAL( "vk_usage_flags is 0" );
			}
			else if ( !cfg.synchronization )
			{
				LOG_CRITICAL( "buffer mediator has expired" );
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
