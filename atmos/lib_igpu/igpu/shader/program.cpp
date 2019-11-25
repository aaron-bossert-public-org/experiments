
#pragma once

#include "igpu/shader/program.h"

#include "igpu/shader/fragment_shader.h"
#include "igpu/shader/vertex_shader.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

size_t program::config::hash() const
{
	size_t h = hash_utils::hash_combine( vertex, fragment );

	hash_utils::hash_combine( &h, constants.hash() );

	return h;
}

ptrdiff_t program::config::compare( const config& other ) const
{
	if ( vertex != other.vertex )
	{
		return vertex.get() - other.vertex.get();
	}
	if ( fragment != other.fragment )
	{
		return fragment.get() - other.fragment.get();
	}

	return constants.compare( other.constants );
}
