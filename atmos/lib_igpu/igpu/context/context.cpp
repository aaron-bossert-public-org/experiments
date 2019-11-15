
#include "igpu/context/context.h"

#include "framework/logging/log.h"

#include < functional >

using namespace igpu;

namespace igpu
{
	class memoizer
	{
	public:
		virtual ~memoizer() = default;
	};
}

namespace
{
	template < typename T >
	class memoizer_impl
		: public memoizer
		, public std::enable_shared_from_this< memoizer_impl< T > >
	{
	public:
		using key_t = typename const T::config*;
		using val_t = std::weak_ptr< T >;
		using make_t = std::function< std::unique_ptr< T >() >;

		std::shared_ptr< T > memoize(
			typename const T::config& cfg,
			make_t&& make )
		{
			auto found = _map.find( &cfg );
			if ( found == _map.end() )
			{
				auto unique = make();
				if ( !unique )
				{
					return nullptr;
				}

				deleter d = { shared_from_this() };
				auto shared = std::shared_ptr< T >( unique.release(), d );

				found = _map.emplace( &shared->cfg(), shared ).first;
			}

			ASSERT_CONTEXT(
				!found->second.expired(),
				"memoized object was not removed by deleter" );
			return found->second.lock();
		}

	private:
		struct hash
		{
			std::size_t operator()( const key_t& k ) const
			{
				return typename T::config::hash( *k );
			}
		};

		struct key_equal
		{
			bool operator()( const key_t& lhs, const key_t& rhs ) const
			{
				return 0 == typename T::config::compare( *lhs, *rhs );
			}
		};

		struct deleter
		{
			std::weak_ptr< memoizer_impl > weak;

			void operator()( T* t ) const
			{
				if ( auto locked = weak.lock() )
				{
					locked->_map.erase( &t->cfg() );
					delete t;
				}
				else
				{
					LOG_CRITICAL(
						"memoizer expired, has context already been "
						"destroyed?" );
				}
			}
		};

		std::unordered_map< key_t, val_t, hash, key_equal > _map;
	};

	size_t s_next_type_index = 0;
	std::vector< std::function< std::shared_ptr< memoizer >() > >
		s_memoizer_generators;

	template < typename T >
	memoizer_impl< T >& get_memoizer(
		std::vector< std::shared_ptr< memoizer > >* memoizers )
	{
		// use static variable to track type as a compact index, use index to
		// look up memoizer and possibly add a new one to memoizers
		static size_t s_type_index = s_next_type_index++;
		if ( s_type_index < s_memoizer_generators.size() )
		{
			LOG_CRITICAL( "unexpected missing type index" );
		}
		else if ( s_type_index == s_memoizer_generators.size() )
		{
			s_memoizer_generators.push_back(
				[]() -> std::shared_ptr< memoizer > {
					return std::make_shared< memoizer_impl< T > >();
				} );
		}

		std::shared_ptr< memoizer >* pp_memoizer =
			&memoizers->at( s_type_index );

		if ( !pp_memoizer )
		{
			*pp_memoizer = s_memoizer_generators[s_type_index]();
		}

		return *(memoizer_impl< T >*)pp_memoizer->get();
	}

	template < typename T >
	std::shared_ptr< T > make_shared(
		std::vector< std::shared_ptr< memoizer > >* memoizers,
		typename const T::config& cfg,
		typename memoizer_impl< T >::make_t&& make )
	{
		auto& memoizer = get_memoizer< T >( memoizers );
		return memoizer.memoize( cfg, std::move( make ) );
	}
}

std::shared_ptr< program > context::make_shared( const program::config& cfg )
{
	return ::make_shared< program >( &_memoizers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< render_states > context::make_shared(
	const render_states::config& cfg )
{
	return ::make_shared< render_states >( &_memoizers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< geometry > context::make_shared( const geometry::config& cfg )
{
	return ::make_shared< geometry >( &_memoizers, cfg, [&] {
		return make( cfg );
	} );
}

std::shared_ptr< primitives > context::make_shared(
	const primitives::config& cfg )
{
	return ::make_shared< primitives >( &_memoizers, cfg, [&] {
		return make( cfg );
	} );
}
