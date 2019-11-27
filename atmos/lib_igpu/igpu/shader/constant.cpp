
#define GLM_ENABLE_EXPERIMENTAL 1

#include "igpu/shader/constant.h"

#include "framework/utility/hash_utils.h"

#include "glm/gtx/hash.hpp"

using namespace igpu;

constant_type constant::type() const
{
	return std::visit(
		[]( auto& value ) { return to_constant_type( value ); },
		value );
}
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

constant_type igpu::to_constant_type( const int32_t& )
{
	return constant_type::INT32;
}

constant_type igpu::to_constant_type( const glm::ivec2& )
{
	return constant_type::IVEC2;
}

constant_type igpu::to_constant_type( const glm::ivec3& )
{
	return constant_type::IVEC3;
}

constant_type igpu::to_constant_type( const glm::ivec4& )
{
	return constant_type::IVEC4;
}

constant_type igpu::to_constant_type( const float& )
{
	return constant_type::F32;
}

constant_type igpu::to_constant_type( const glm::vec2& )
{
	return constant_type::VEC2;
}

constant_type igpu::to_constant_type( const glm::vec3& )
{
	return constant_type::VEC3;
}

constant_type igpu::to_constant_type( const glm::vec4& )
{
	return constant_type::VEC4;
}