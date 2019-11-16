
#include "vulkan/context/vulkan_abandon_manager.h"

#include "framework/logging/log.h"

using namespace igpu;

namespace
{
	using config = vulkan_abandon_manager::config;
	using payload = vulkan_abandon_manager::payload;
	using destroyer_t = std::function< void( const config&, payload& ) >;

	std::vector< destroyer_t > s_destroyers;

	template < typename... ARGS >
	size_t register_destroyer( const destroyer_t& destroyer )
	{
		size_t index = s_destroyers.size();

		s_destroyers.push_back( destroyer );

		return index;
	}

	template < typename... ARGS >
	void abandon(
		size_t type_index,
		std::vector< std::vector< payload > >* p_abandoned,
		ARGS... args )
	{
		if ( p_abandoned->size() <= type_index )
		{
			p_abandoned->resize( type_index + 1 );
		}

		p_abandoned->at( type_index ).push_back( { args... } );
	}
}

void vulkan_abandon_manager::destroy_abandoned()
{
	for ( size_t i = 0; i < _abandoned.size(); ++i )
	{
		for ( payload& p : _abandoned[i] )
		{
			s_destroyers[i]( _cfg, p );
		}
	}
}

vulkan_abandon_manager::~vulkan_abandon_manager()
{
	destroy_abandoned();
}

std::unique_ptr< vulkan_abandon_manager > vulkan_abandon_manager::make(
	const config& cfg )
{
	if ( !cfg.device )
	{
		LOG_CRITICAL( "device is nulll" );
	}
	else
	{
		return std::unique_ptr< vulkan_abandon_manager >(
			new vulkan_abandon_manager( cfg ) );
	}

	return nullptr;
}


template <>
void vulkan_abandon_manager::abandon< VkBuffer >(
	VkBuffer buffer,
	VmaAllocation vma_allocation )
{
	static size_t s_type_index =
		register_destroyer< VkBuffer >( []( const config& cfg, payload& p ) {
			vmaDestroyBuffer( cfg.vma, (VkBuffer)p._0, (VmaAllocation)p._1 );
		} );

	::abandon( s_type_index, &_abandoned, buffer, vma_allocation );
}

template <>
void vulkan_abandon_manager::abandon< VkImage >( VkImage image )
{
	static size_t s_type_index =
		register_destroyer< VkBuffer >( []( const config& cfg, payload& p ) {
			vkDestroyImage( cfg.device, (VkImage)p._0, nullptr );
		} );

	::abandon( s_type_index, &_abandoned, image );
}