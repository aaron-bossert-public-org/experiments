
#include "igpu/context/context.h"

#include "framework/logging/log.h"

#include < functional >

using namespace igpu;

namespace igpu
{
	class tracker
	{
	public:
		virtual ~tracker() = default;
	};
}

namespace
{
	template < typename T >
	class visitable : public tracker
	{
	public:
		virtual void visit( const std::function< void( T* ) >& ) = 0;
	};

	template <
		typename T,
		typename KEY_T,
		typename HASHER_T,
		typename KEYIFIER_T >
	class tracker_impl
		: public visitable< T >
		, public std::enable_shared_from_this<
			  tracker_impl< T, KEY_T, HASHER_T, KEYIFIER_T > >
	{
	public:
		std::shared_ptr< T > find( KEY_T key )
		{
			auto found = _map.find( key );
			if ( found == _map.end() )
			{
				return nullptr;
			}

			ASSERT_CONTEXT(
				!found->second.expired(),
				"memoized object was not removed by deleter" );
			return found->second.lock();
		}

		std::shared_ptr< T > emplace( KEY_T key, std::unique_ptr< T >&& unique )
		{
			deleter d = { shared_from_this() };
			auto shared = std::shared_ptr< T >( unique.release(), d );

			_map.emplace( key, shared );

			return shared;
		}

		void visit( const std::function< void( T* ) >& f ) override
		{
			for ( const auto& pair : _map )
			{
				f( pair.second.lock().get() );
			}
		}

	private:
		struct deleter
		{
			std::weak_ptr< tracker_impl > weak;

			void operator()( T* t ) const
			{
				if ( auto locked = weak.lock() )
				{
					KEY_T key = KEYIFIER_T::keyify( t );
					locked->_map.erase( key );
					delete t;
				}
				else
				{
					LOG_CRITICAL(
						"tracker expired, has context already been "
						"destroyed?" );
				}
			}
		};

		std::unordered_map< KEY_T, std::weak_ptr< T >, HASHER_T, KEYIFIER_T >
			_map;
	};

	size_t s_next_type_index = 0;
	std::vector< std::function< std::shared_ptr< tracker >() > >
		s_tracker_generators;


	template < typename T >
	size_t type_index()
	{
		// use static variable to track type as a compact index, use index to
		// look up tracker and possibly add a new one to trackers
		static size_t s_type_index = s_next_type_index++;
		return s_type_index;
	}

	template < typename T >
	T* get_tracker( std::vector< std::shared_ptr< tracker > >* trackers )
	{
		size_t type_index = ::type_index< T >();
		if ( type_index >= trackers->size() )
		{
			trackers->resize( type_index + 1 );
		}

		std::shared_ptr< tracker >* pp_tracker = &trackers->at( type_index );

		if ( !*pp_tracker )
		{
			*pp_tracker = std::make_shared< T >();
		}

		return ASSERT_CAST( T*, pp_tracker->get() );
	}

	template < typename T >
	void visit(
		const std::vector< std::shared_ptr< tracker > >& trackers,
		const std::function< void( T* ) >& visitor )
	{
		size_t type_index = ::type_index< T >();

		if ( type_index < trackers.size() )
		{
			if ( auto* tracker = ASSERT_CAST(
					 visitable< T >*,
					 trackers[type_index].get() ) )
			{
				tracker->visit( visitor );
			}
		}
	}

	template <
		typename T,
		typename MAKE_T,
		typename CONFIG_T = typename const T::config >
	std::shared_ptr< T > memoize(
		std::vector< std::shared_ptr< tracker > >* trackers,
		CONFIG_T& cfg,
		MAKE_T&& make )
	{
		struct hasher
		{
			std::size_t operator()( const CONFIG_T* k ) const
			{
				return k->hash();
			}
		};

		struct keyifier
		{
			static const CONFIG_T* keyify( const T* t )
			{
				return &t->cfg();
			}

			bool operator()( const CONFIG_T* lhs, const CONFIG_T* rhs ) const
			{
				return lhs->compare( *rhs );
			}
		};

		using tracker_t = tracker_impl< T, const CONFIG_T*, hasher, keyifier >;

		if ( tracker_t* tracker = get_tracker< tracker_t >( trackers ) )
		{
			if ( std::shared_ptr< T > shared = tracker->find( &cfg ) )
			{
				return shared;
			}
			if ( auto unique = make() )
			{
				return tracker->emplace(
					keyifier().keyify( unique.get() ),
					std::move( unique ) );
			}
		}

		return nullptr;
	}

	template < typename T >
	std::shared_ptr< T > emplace(
		std::vector< std::shared_ptr< tracker > >* trackers,
		std::unique_ptr< T >&& unique )
	{
		if ( unique )
		{
			struct keyifier : std::equal_to< const T* >
			{
				static const T* keyify( const T* t )
				{
					return t;
				}
			};

			using tracker_t =
				tracker_impl< T, const T*, std::hash< const T* >, keyifier >;

			if ( tracker_t* tracker = get_tracker< tracker_t >( trackers ) )
			{
				const T* t = unique.get();
				return tracker->emplace( t, std::move( unique ) );
			}

			LOG_CRITICAL( "failed to find tracker" );
		}
		return nullptr;
	}
}

std::shared_ptr< program > context::make_shared( const program::config& cfg )
{
	return ::memoize< program >( &_trackers, cfg, [&] { return make( cfg ); } );
}

std::shared_ptr< render_states > context::make_shared(
	const render_states::config& cfg )
{
	return ::memoize< render_states >( &_trackers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< geometry > context::make_shared( const geometry::config& cfg )
{
	return ::memoize< geometry >( &_trackers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< primitives > context::make_shared(
	const primitives::config& cfg )
{
	return ::memoize< primitives >( &_trackers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< graphics_pipeline > context::make_shared(
	const graphics_pipeline::config& cfg )
{
	return ::memoize< graphics_pipeline >( &_trackers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< draw_target > context::make_shared(
	const draw_target::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< render_buffer > context::make_shared(
	const render_buffer::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< render_texture2d > context::make_shared(
	const render_texture2d::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< depth_buffer > context::make_shared(
	const depth_buffer::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< depth_texture2d > context::make_shared(
	const depth_texture2d::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< vertex_shader > context::make_shared(
	const vertex_shader::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< fragment_shader > context::make_shared(
	const fragment_shader::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< vertex_buffer > context::make_shared(
	const vertex_buffer::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< index_buffer > context::make_shared(
	const index_buffer::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< compute_buffer > context::make_shared(
	const compute_buffer::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< texture2d > context::make_shared(
	const texture2d::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< opaque_batch > context::make_shared(
	const opaque_batch::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

std::shared_ptr< transparent_batch > context::make_shared(
	const transparent_batch::config& cfg )
{
	return ::emplace( &_trackers, make( cfg ) );
}

template <>
void context::visit< program >(
	const std::function< void( program* ) >& visitor )
{
	::visit< program >( _trackers, visitor );
}

template <>
void context::visit< render_states >(
	const std::function< void( render_states* ) >& visitor )
{
	::visit< render_states >( _trackers, visitor );
}

template <>
void context::visit< geometry >(
	const std::function< void( geometry* ) >& visitor )
{
	::visit< geometry >( _trackers, visitor );
}

template <>
void context::visit< primitives >(
	const std::function< void( primitives* ) >& visitor )
{
	::visit< primitives >( _trackers, visitor );
}

template <>
void context::visit< graphics_pipeline >(
	const std::function< void( graphics_pipeline* ) >& visitor )
{
	::visit< graphics_pipeline >( _trackers, visitor );
}

template <>
void context::visit< draw_target >(
	const std::function< void( draw_target* ) >& visitor )
{
	::visit< draw_target >( _trackers, visitor );
}

template <>
void context::visit< render_buffer >(
	const std::function< void( render_buffer* ) >& visitor )
{
	::visit< render_buffer >( _trackers, visitor );
}

template <>
void context::visit< render_texture2d >(
	const std::function< void( render_texture2d* ) >& visitor )
{
	::visit< render_texture2d >( _trackers, visitor );
}

template <>
void context::visit< depth_buffer >(
	const std::function< void( depth_buffer* ) >& visitor )
{
	::visit< depth_buffer >( _trackers, visitor );
}

template <>
void context::visit< depth_texture2d >(
	const std::function< void( depth_texture2d* ) >& visitor )
{
	::visit< depth_texture2d >( _trackers, visitor );
}

template <>
void context::visit< vertex_shader >(
	const std::function< void( vertex_shader* ) >& visitor )
{
	::visit< vertex_shader >( _trackers, visitor );
}

template <>
void context::visit< fragment_shader >(
	const std::function< void( fragment_shader* ) >& visitor )
{
	::visit< fragment_shader >( _trackers, visitor );
}

template <>
void context::visit< vertex_buffer >(
	const std::function< void( vertex_buffer* ) >& visitor )
{
	::visit< vertex_buffer >( _trackers, visitor );
}

template <>
void context::visit< index_buffer >(
	const std::function< void( index_buffer* ) >& visitor )
{
	::visit< index_buffer >( _trackers, visitor );
}

template <>
void context::visit< compute_buffer >(
	const std::function< void( compute_buffer* ) >& visitor )
{
	::visit< compute_buffer >( _trackers, visitor );
}

template <>
void context::visit< texture2d >(
	const std::function< void( texture2d* ) >& visitor )
{
	::visit< texture2d >( _trackers, visitor );
}

template <>
void context::visit< opaque_batch >(
	const std::function< void( opaque_batch* ) >& visitor )
{
	::visit< opaque_batch >( _trackers, visitor );
}

template <>
void context::visit< transparent_batch >(
	const std::function< void( transparent_batch* ) >& visitor )
{
	::visit< transparent_batch >( _trackers, visitor );
}
