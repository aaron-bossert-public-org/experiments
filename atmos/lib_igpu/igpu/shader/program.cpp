
#pragma once

#include "igpu/shader/program.h"

#include "igpu/shader/fragment_shader.h"
#include "igpu/shader/vertex_shader.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

size_t program::config::hash( const config& cfg )
{
	return hash_utils::hash_combine( cfg.vertex, cfg.fragment );
}

ptrdiff_t program::config::compare( const config& lhs, const config& rhs )
{
	if ( lhs.vertex != rhs.vertex )
	{
		return lhs.vertex.get() - rhs.vertex.get();
	}

	return lhs.fragment.get() - rhs.fragment.get();
}
