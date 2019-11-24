
#include "igpu/shader/primitives.h"

#include "framework/logging/log.h"
#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

bool primitives::find_expected(
	const std::string_view& name,
	size_t* p_expected ) const
{
	size_t count = primitive_count();
	size_t expected = *p_expected;

	if ( 0 == count )
	{
		return false;
	}

	if ( expected >= count )
	{
		expected = 0;
	}

	size_t at = expected;

	do
	{
		if ( primitive( at ).cfg().name == name )
		{
			*p_expected = at;
			return true;
		}
		at = ( at + 1 ) % count;
	} while ( at != expected );

	return false;
}

size_t primitives::config::hash() const
{
	size_t h = 0;

	for ( size_t i = 0; i < primitives.size(); ++i )
	{
		hash_utils::hash_combine( &h, primitives[i].hash() );
	}

	return h;
}

ptrdiff_t primitives::config::compare( const config& other ) const
{
	size_t prim_count = std::min( primitives.size(), other.primitives.size() );

	for ( size_t i = 0; i < prim_count; ++i )
	{
		if ( ptrdiff_t prim_cmp = primitives[i] .compare(
				 other.primitives[i] ) )
		{
			return prim_cmp;
		}
	}

	return primitives.size() - other.primitives.size();
}
