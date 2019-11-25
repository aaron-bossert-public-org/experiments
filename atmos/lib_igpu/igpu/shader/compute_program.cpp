
#pragma once

#include "igpu/shader/compute_program.h"

#include "igpu/shader/compute_shader.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

size_t compute_program::config::hash() const
{
	size_t h = hash_utils::hash( compute );

	hash_utils::hash_combine( &h, constants.hash() );

	return h;
}

ptrdiff_t compute_program::config::compare( const config& other ) const
{
	if ( compute != other.compute )
	{
		return compute.get() - other.compute.get();
	}

	return constants.compare( other.constants );
}
