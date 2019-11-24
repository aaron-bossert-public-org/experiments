
#include "igpu/shader/constants.h"

#include "framework/logging/log.h"
#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

size_t constants::config::find( const std::string_view& name ) const
{
	for ( size_t i = 0; i < constants.size(); ++i )
	{
		if ( constants[i].name == name )
		{
			return i;
		}
	}
	return ~0U;
}

size_t constants::config::hash() const
{
	size_t h = 0;

	for ( size_t i = 0; i < constants.size(); ++i )
	{
		hash_utils::hash_combine( &h, constants[i].hash() );
	}

	return h;
}

ptrdiff_t constants::config::compare( const config& other ) const
{
	size_t prim_count = std::min( constants.size(), other.constants.size() );

	for ( size_t i = 0; i < prim_count; ++i )
	{
		if ( ptrdiff_t prim_cmp = constants[i].compare( other.constants[i] ) )
		{
			return prim_cmp;
		}
	}

	return constants.size() - other.constants.size();
}
