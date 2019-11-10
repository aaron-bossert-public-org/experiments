
#include "vulkan/buffer/vulkan_index_buffer.h"

#include "vulkan/buffer/vulkan_staged_buffer.h"

using namespace igpu;

VkIndexType igpu::to_vulkan_format( index_format format )
{
	switch ( format )
	{
	case index_format::UNSIGNED_INT:
		return VK_INDEX_TYPE_UINT32;
	case index_format::UNSIGNED_SHORT:
		return VK_INDEX_TYPE_UINT16;
	};

	if ( is_valid( format ) )
	{
		LOG_CRITICAL( "unhandled format: %s", to_string( format ).data() );
	}
	else
	{
		LOG_CRITICAL( "invalid format: %d", format );
	}

	return VK_INDEX_TYPE_UINT16;
}

size_t vulkan_index_buffer::element_count() const
{
	return byte_size() / bytes_per_index( cfg().format );
}

const vulkan_index_buffer::config& vulkan_index_buffer::cfg() const
{
	return _cfg;
}

std::unique_ptr< vulkan_index_buffer > vulkan_index_buffer::make(
	const config& cfg,
	const scoped_ptr< vulkan_synchronization >& synchronization )
{
	if ( !is_valid( cfg.format ) )
	{
		LOG_CRITICAL( "invalid format: %d", cfg.format );
	}
	else
	{
		return vulkan_staged_buffer_t< vulkan_index_buffer >::make(
			{
				cfg.vk.device_properties,
				VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
				synchronization,
				cfg.memory,
			},
			cfg );
	}
	return nullptr;
}

vulkan_index_buffer::vulkan_index_buffer( const config& cfg )
	: _cfg( cfg )
{}