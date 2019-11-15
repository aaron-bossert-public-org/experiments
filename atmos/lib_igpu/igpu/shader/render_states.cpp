
#pragma once

#include "igpu/shader/render_states.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

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

size_t render_states::config::hash( const config& cfg )
{
	std::string_view sv( (char*)&cfg, sizeof cfg );
	return hash_utils::hash( sv );
}

ptrdiff_t render_states::config::compare( const config& lhs, const config& rhs )
{
	return mem_compare( lhs, rhs );
}
