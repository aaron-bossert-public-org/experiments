
#include <igpu/shader/primitive.h>

#include <igpu/texture/depth_texture2d.h>
#include <igpu/texture/texture2d.h>

using namespace igpu;

parameter::type primitive::type() const
{

#define CASE_STATIC_ASSERT(I, T, E) \
	case I:\
	static_assert( std::is_same_v< std::decay_t< decltype( std::get< I >(variant_t()) ) >, std::shared_ptr< igpu::T > > ); \
	return parameter::type::E;

	
	switch (cfg().value.index())
	{
		CASE_STATIC_ASSERT(0, compute_buffer, COMPUTE_BUFFER);
		CASE_STATIC_ASSERT(1, texture2d, TEXTURE2D);
		CASE_STATIC_ASSERT(2, depth_texture2d, TEXTURE2D);
		CASE_STATIC_ASSERT(3, render_texture2d, TEXTURE2D);
	};

	return parameter::type::UNDEFINED;
}