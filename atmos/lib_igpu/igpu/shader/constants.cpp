
#include "igpu/shader/constants.h"

#include "framework/logging/log.h"
#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

size_t constants::config::find( const std::string_view& name ) const
{
	for ( size_t i = 0; i < size(); ++i )
	{
		if ( at( i ).name == name )
		{
			return i;
		}
	}
	return ~0U;
}

size_t constants::config::hash() const
{
	size_t h = 0;

	for ( size_t i = 0; i < size(); ++i )
	{
		hash_utils::hash_combine( &h, at( i ).hash() );
	}

	return h;
}

ptrdiff_t constants::config::compare( const config& other ) const
{
	size_t prim_count = std::min( size(), other.size() );

	for ( size_t i = 0; i < prim_count; ++i )
	{
		if ( auto cmp = at( i ).compare( other.at( i ) ) )
		{
			return cmp;
		}
	}

	return size() - other.size();
}
