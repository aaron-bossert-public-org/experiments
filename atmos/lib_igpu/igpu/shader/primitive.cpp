
#include "igpu/shader/primitive.h"

#include "igpu/texture/depth_texture2d.h"
#include "igpu/texture/texture2d.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

parameter::type primitive::type() const
{
#define CASE_STATIC_ASSERT( I, T, E )                                     \
	case I:                                                               \
		static_assert(                                                    \
			std::is_same_v<                                               \
				std::decay_t< decltype( std::get< I >( variant_t() ) ) >, \
				std::shared_ptr< igpu::T > > );                           \
		return parameter::type::E;

	switch ( cfg().value.index() )
	{
		CASE_STATIC_ASSERT( 0, compute_buffer, UNIFORM_BUFFER );
		CASE_STATIC_ASSERT( 1, vertex_buffer, STORAGE_BUFFER );
		CASE_STATIC_ASSERT( 2, index_buffer, STORAGE_BUFFER );
		CASE_STATIC_ASSERT( 3, texture2d, SAMPLER2D );
		CASE_STATIC_ASSERT( 4, depth_texture2d, SAMPLER2D );
		CASE_STATIC_ASSERT( 5, render_texture2d, SAMPLER2D );
	};

	return parameter::type::UNDEFINED;
}

size_t primitive::config::hash() const
{
	size_t h = hash_utils::hash( name );

	std::visit(
		[&]( auto&& shared ) { hash_utils::hash_combine( &h, shared ); },
		value );

	return h;
}

ptrdiff_t primitive::config::compare( const config& other ) const
{
	if ( int name_cmp = name.compare( other.name ) )
	{
		return name_cmp;
	}

	const char* l_addr = std::visit(
		[&]( auto&& shared ) { return (const char*)shared.get(); },
		value );

	const char* r_addr = std::visit(
		[&]( auto&& shared ) { return (const char*)shared.get(); },
		other.value );

	return l_addr - r_addr;
}
