
#pragma once

#include <functional>

namespace hash_utils
{
	template < typename T >
	size_t hash( const T& t )
	{
		return std::hash< T >()( t );
	}

	template < typename... ARGS >
	size_t hash_combine( size_t* seed, size_t value )
	{
		return *seed ^= value + ( *seed << 6 ) + ( *seed >> 2 ) +
			(size_t)0x9e3779b97f4a7c13LL;
	}

	template < typename... ARGS >
	size_t hash_combine( ARGS&&... args )
	{
		size_t seed = 0;
		( hash_combine( &seed, hash( args ) ), ... );
		return seed;
	}

	// c_hash(nullptr) 0, c_hash("") == seed * prime
	constexpr uint32_t c_slow_str_hash_32(
		const char* const str,
		uint32_t seed = 2166136261U,
		uint32_t i = 0 )
	{
		if ( str )
		{
			uint64_t prime = 16777619U;
			return str[i] == 0
				? uint32_t( seed * prime )
				: c_slow_str_hash_32(
					  str,
					  uint32_t( ( seed ^ (uint32_t)str[i] ) * prime ),
					  i + 1 );
		}

		return 0;
	}
}