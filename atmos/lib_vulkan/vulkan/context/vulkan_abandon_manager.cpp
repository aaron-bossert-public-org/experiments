
#include "vulkan/context/vulkan_abandon_manager.h"

#include "framework/logging/log.h"

using namespace igpu;

namespace
{
	using config = vulkan_abandon_manager::config;
	using payload = vulkan_abandon_manager::payload;
	using destroyer_t = std::function< void( const config&, payload& ) >;

	std::vector< destroyer_t > s_destroyers;

	template <
		typename T,
		typename DESTROYER_T = std::function< void( const config&, T ) > >
	size_t register_destroyer( T, const DESTROYER_T& destroyer )
	{
		size_t index = s_destroyers.size();

		s_destroyers.push_back( [destroyer]( const config& cfg, payload& p ) {
			destroyer( cfg, (T)p._0 );
		} );

		return index;
	}

	template <
		typename T,
		typename U,
		typename DESTROYER_T = std::function< void( const config&, T, U ) > >
	size_t register_destroyer( T, U, const DESTROYER_T& destroyer )
	{
		size_t index = s_destroyers.size();

		s_destroyers.push_back( [destroyer]( const config& cfg, payload& p ) {
			destroyer( cfg, (T)p._0, (U)p._1 );
		} );

		return index;
	}

	template < typename... ARGS >
	void abandon(
		size_t type_index,
		std::vector< std::vector< payload > >* p_abandoned,
		ARGS... args )
	{
		if ( ( args && ... ) )
		{
			if ( p_abandoned->size() <= type_index )
			{
				p_abandoned->resize( type_index + 1 );
			}

			p_abandoned->at( type_index ).push_back( { args... } );
		}
		else if ( ( args || ... ) )
		{
			LOG_CRITICAL(
				"args for abandon must all be null or all be non null" );
		}
	}
}

void vulkan_abandon_manager::swap_frame()
{
	_swap_index = ( _swap_index + 1 ) % _abandoned.size();
	destroy_abandoned( _swap_index );
}

vulkan_abandon_manager::~vulkan_abandon_manager()
{
	for ( size_t i = 0; i < _abandoned.size(); ++i )
	{
		destroy_abandoned( i );
	}
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

vulkan_abandon_manager::vulkan_abandon_manager( const config& cfg )
	: _cfg( cfg )
	, _abandoned( cfg.swap_count )
{}

void vulkan_abandon_manager::destroy_abandoned( size_t swap_index )
{
	auto& abandoned = _abandoned[swap_index];
	for ( size_t i = 0; i < abandoned.size(); ++i )
	{
		for ( payload& p : abandoned[i] )
		{
			s_destroyers[i]( _cfg, p );
		}
	}
	abandoned.clear();
}

std::vector< std::vector< vulkan_abandon_manager::payload > >*
	vulkan_abandon_manager::abandoned_frame()
{
	return &_abandoned[_swap_index];
}

template <>
void vulkan_abandon_manager::abandon<>(
	VkBuffer buffer,
	VmaAllocation vma_allocation )
{
	static size_t s_type_index = register_destroyer(
		buffer,
		vma_allocation,
		[]( const config& cfg, VkBuffer buff, VmaAllocation alloc ) {
			vmaDestroyBuffer( cfg.vma, buff, alloc );
		} );

	::abandon( s_type_index, abandoned_frame(), buffer, vma_allocation );
}

template <>
void vulkan_abandon_manager::abandon<>( VkImage obj )
{
	static size_t s_type_index =
		register_destroyer( obj, []( const config& cfg, auto obj ) {
			vkDestroyImage( cfg.device, obj, nullptr );
		} );

	::abandon( s_type_index, abandoned_frame(), obj );
}

template <>
void vulkan_abandon_manager::abandon<>( VkSampler obj )
{
	static size_t s_type_index =
		register_destroyer( obj, []( const config& cfg, auto obj ) {
			vkDestroySampler( cfg.device, obj, nullptr );
		} );

	::abandon( s_type_index, abandoned_frame(), obj );
}

template <>
void vulkan_abandon_manager::abandon<>( VkImageView obj )
{
	static size_t s_type_index =
		register_destroyer( obj, []( const config& cfg, auto obj ) {
			vkDestroyImageView( cfg.device, obj, nullptr );
		} );

	::abandon( s_type_index, abandoned_frame(), obj );
}

template <>
void vulkan_abandon_manager::abandon<>( VkDeviceMemory obj )
{
	static size_t s_type_index =
		register_destroyer( obj, []( const config& cfg, auto obj ) {
			vkFreeMemory( cfg.device, obj, nullptr );
		} );

	::abandon( s_type_index, abandoned_frame(), obj );
}

template <>
void vulkan_abandon_manager::abandon<>(
	VkCommandPool pool,
	VkCommandBuffer command_buffer )
{
	static size_t s_type_index = register_destroyer(
		pool,
		command_buffer,
		[]( const config& cfg, auto pool, auto command_buffer ) {
			vkFreeCommandBuffers( cfg.device, pool, 1, &command_buffer );
		} );

	::abandon( s_type_index, abandoned_frame(), pool, command_buffer );
}
