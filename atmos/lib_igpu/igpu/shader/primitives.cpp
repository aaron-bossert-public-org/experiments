
#include "igpu/shader/primitives.h"

#include "framework/logging/log.h"
#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

bool primitives::find_expected( const std::string& name, size_t* p_expected )
	const
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


size_t primitives::config::hash( const config& cfg )
{
	size_t h = 0;

	for ( size_t i = 0; i < cfg.primitives.size(); ++i )
	{
		hash_utils::hash_combine( &h, cfg.primitives[i].name );

		std::visit(
			[&]( auto&& shared ) { hash_utils::hash_combine( &h, shared ); },
			cfg.primitives[i].value );
	}

	return h;
}

ptrdiff_t primitives::config::compare( const config& lhs, const config& rhs )
{
	size_t prim_count =
		std::min( lhs.primitives.size(), rhs.primitives.size() );

	for ( size_t i = 0; i < prim_count; ++i )
	{
		const primitive::config& l_prim = lhs.primitives[i];
		const primitive::config& r_prim = rhs.primitives[i];


		if ( int name_cmp = l_prim.name.compare( r_prim.name ) )
		{
			return name_cmp;
		}

		const char* l_addr = std::visit(
			[&]( auto&& shared ) { return (const char*)shared.get(); },
			l_prim.value );

		const char* r_addr = std::visit(
			[&]( auto&& shared ) { return (const char*)shared.get(); },
			rhs.primitives[i].value );

		if ( l_addr != r_addr )
		{
			return l_addr - r_addr;
		}
	}

	return lhs.primitives.size() - rhs.primitives.size();
}
