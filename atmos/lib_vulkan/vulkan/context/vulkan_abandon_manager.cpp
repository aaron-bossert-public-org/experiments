
#include "vulkan/context/vulkan_abandon_manager.h"

#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_queue.h"

#include "framework/logging/log.h"

using namespace igpu;

namespace
{
	using payload_t = vulkan_abandon_manager::payload_t;
	using destroyer_t = std::function< void( payload_t& ) >;

	std::vector< destroyer_t > s_destroyers;

	bool null_check( VmaAllocator )
	{
		return false;
	}

	bool null_check( VkDevice )
	{
		return false;
	}

	bool null_check( void* ptr )
	{
		return ptr != nullptr;
	}

	template < typename... ARGS >
	struct abandon_impl
	{
		size_t type_index;

		void operator()( vulkan_queue* queue, ARGS... args )
		{
			if ( ( args && ... ) )
			{
				auto tup = std::make_tuple( args... );
				payload_t& p = *( (payload_t*)&tup );
				static_assert( sizeof( tup ) <= sizeof( p ) );

				if ( queue )
				{
					auto& pending = queue->abandon_manager().pending();

					if ( pending.size() <= type_index )
					{
						pending.resize( type_index + 1 );
					}

					pending.at( type_index ).push( p );
				}
				else
				{
					s_destroyers[type_index]( p );
				}
			}
			else if ( ( null_check( args ) || ... ) )
			{
				LOG_CRITICAL(
					"args for abandon must all be null or all be non null" );
			}
		}
	};

	template <
		typename... ARGS,
		typename DESTROYER_T = std::function< void( ARGS... ) > >
	abandon_impl< ARGS... > register_destroyer(
		const DESTROYER_T& destroyer,
		ARGS... args )
	{
		size_t index = s_destroyers.size();

		s_destroyers.push_back( [destroyer]( payload_t& p ) {
			using tuple_t = std::tuple< ARGS... >;
			static_assert( sizeof( tuple_t ) <= sizeof( payload_t ) );

			auto tuple = (tuple_t*)&p;

			destroyer( std::get< decltype( args ) >( *tuple )... );
		} );

		return { index };
	}

}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VmaAllocator vma,
	VkBuffer buff,
	VmaAllocation alloc )
{
	static auto impl = register_destroyer(
		[]( auto vma, auto buff, auto alloc ) {
			vmaDestroyBuffer( vma, buff, alloc );
		},
		vma,
		buff,
		alloc );

	impl( queue.get(), vma, buff, alloc );
}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VkDevice device,
	VkImage obj )
{
	static auto impl = register_destroyer(
		[]( auto device, auto obj ) { vkDestroyImage( device, obj, nullptr ); },
		device,
		obj );

	impl( queue.get(), device, obj );
}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VkDevice device,
	VkSampler obj )
{
	static auto impl = register_destroyer(
		[]( auto device, auto obj ) {
			vkDestroySampler( device, obj, nullptr );
		},
		device,
		obj );

	impl( queue.get(), device, obj );
}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VkDevice device,
	VkImageView obj )
{
	static auto impl = register_destroyer(
		[]( auto device, auto obj ) {
			vkDestroyImageView( device, obj, nullptr );
		},
		device,
		obj );

	impl( queue.get(), device, obj );
}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VkDevice device,
	VkDeviceMemory obj )
{
	static auto impl = register_destroyer(
		[]( auto device, auto obj ) { vkFreeMemory( device, obj, nullptr ); },
		device,
		obj );

	impl( queue.get(), device, obj );
}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VkDevice device,
	VkCommandPool pool,
	VkCommandBuffer cb )
{
	static auto impl = register_destroyer(
		[]( auto device, auto pool, auto cb ) {
			vkFreeCommandBuffers( device, pool, 1, &cb );
		},
		device,
		pool,
		cb );

	impl( queue.get(), device, pool, cb );
}

template <>
void igpu::abandon<>(
	const scoped_ptr< vulkan_queue >& queue,
	VkDevice device,
	VkCommandPool pool )
{
	static auto impl = register_destroyer(
		[]( auto device, auto pool ) {
			vkDestroyCommandPool( device, pool, nullptr );
		},
		device,
		pool );

	impl( queue.get(), device, pool );
}

void vulkan_abandon_manager::trigger_abandon(
	const std::shared_ptr< vulkan_fence >& fence )
{
	if ( !fence )
	{
		LOG_CRITICAL( "fence is null" );
	}
	else
	{
		abandoned abandon;
		for ( size_t type_index = 0; type_index < _pending.size();
			  ++type_index )
		{
			if ( !_pending[type_index].empty() )
			{
				abandon.categories.push_back( { type_index } );
				abandon.categories.back().payloads =
					std::move( _pending[type_index] );
			}
		}

		if ( abandon.categories.size() )
		{
			abandon.fence = fence;
			abandon.submit_index = fence->submit_index();
			_abandoned.push( std::move( abandon ) );
		}
	}

	while ( !_abandoned.empty() )
	{
		abandoned& abandon = _abandoned.front();

		if ( !abandon.fence->is_ready( abandon.submit_index ) )
		{
			break;
		}

		for ( auto& category : abandon.categories )
		{
			while ( !category.payloads.empty() )
			{
				s_destroyers[category.type_index]( category.payloads.front() );

				category.payloads.pop();
			}
		}

		_abandoned.pop();
	}
}

vulkan_abandon_manager::pending_t& vulkan_abandon_manager::pending()
{
	return _pending;
}

vulkan_abandon_manager::~vulkan_abandon_manager()
{
	while ( !_abandoned.empty() )
	{
		abandoned& abandon = _abandoned.front();
		abandon.fence->wait( abandon.submit_index );
		for ( auto& category : abandon.categories )
		{
			while ( !category.payloads.empty() )
			{
				s_destroyers[category.type_index]( category.payloads.front() );

				category.payloads.pop();
			}
		}
		_abandoned.pop();
	}

	for ( size_t type_index = 0; type_index < _pending.size(); ++type_index )
	{
		auto& payloads = _pending[type_index];
		while ( !payloads.empty() )
		{
			s_destroyers[type_index]( payloads.front() );
			payloads.pop();
		}
	}
}

std::unique_ptr< vulkan_abandon_manager > vulkan_abandon_manager::make()
{
	return std::unique_ptr< vulkan_abandon_manager >(
		new vulkan_abandon_manager() );
}

vulkan_abandon_manager::vulkan_abandon_manager()
{}
