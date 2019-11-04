
#pragma once

#include "igpu/shader/render_states.h"


int igpu::mem_compare(
	const render_states::config& lhs,
	const render_states::config& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( lhs ) );
}

int igpu::mem_compare( const cull_mode& lhs, const cull_mode& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( lhs ) );
}

int igpu::mem_compare( const blend_mode& lhs, const blend_mode& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( lhs ) );
}

int igpu::mem_compare( const stencil_mode& lhs, const stencil_mode& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( lhs ) );
}

int igpu::mem_compare( const depth_mode& lhs, const depth_mode& rhs )
{
	return memcmp( &lhs, &rhs, sizeof( lhs ) );
}
