
#define GLM_ENABLE_EXPERIMENTAL 1

#include "igpu/shader/constant.h"

#include "framework/utility/hash_utils.h"

#include "glm/gtx/hash.hpp"

using namespace igpu;

size_t constant::hash() const
{
	size_t h = hash_utils::hash( name );

	std::visit(
		[&]( auto&& value ) {
			hash_utils::hash_combine( &h, hash_utils::hash( value ) );
		},
		value );

	return h;
}

ptrdiff_t constant::compare( const constant& other ) const
{
	if ( int name_cmp = name.compare( other.name ) )
	{
		return name_cmp;
	}

	if ( value.index() != other.value.index() )
	{
		return value.index() - other.value.index();
	}

	return std::visit(
		[&]( auto& value ) {
			using type_t = decltype( value );
			using decay_t = std::decay< type_t >::type;

			return memcmp(
				&value,
				&std::get< decay_t >( other.value ),
				sizeof value );
		},
		value );
}
