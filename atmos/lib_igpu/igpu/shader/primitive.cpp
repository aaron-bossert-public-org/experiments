
#include "igpu/shader/primitive.h"

#include "igpu/texture/depth_texture2d.h"
#include "igpu/texture/texture2d.h"

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