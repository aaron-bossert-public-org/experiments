

#include "gl/buffer/gl_index_buffer.h"

#include "gl/buffer/gl_buffer.h"
#include "gl/defines/gl_includes.h"

using namespace igpu;

namespace
{
	unsigned to_gl_format( index_format format )
	{
		switch ( format )
		{
		case index_format::UNSIGNED_SHORT:
			return GL_UNSIGNED_SHORT;
		case index_format::UNSIGNED_INT:
			return GL_UNSIGNED_INT;
		};

		LOG_CRITICAL(
			"unhandled index_format value: %s",
			to_string( format ).data() );

		return GL_FALSE;
	}
}

unsigned gl_index_buffer::gl_format() const
{
	return _gl_format;
}

size_t gl_index_buffer::element_count() const
{
	return byte_size() / bytes_per_index( cfg().format );
}

std::unique_ptr< gl_index_buffer > gl_index_buffer::make( const config& cfg )
{
	if ( !is_valid( cfg.format ) )
	{
		LOG_CRITICAL( "invalid format:%d", (int)cfg.format );
	}
	else
	{
		unsigned gl_format = to_gl_format( cfg.format );

		if ( GL_FALSE == gl_format )
		{
			LOG_CRITICAL(
				"unhandled format:%s",
				to_string( cfg.format ).data() );
		}
		else
		{
			return gl_buffer_t< gl_index_buffer >::
				make( cfg, GL_ELEMENT_ARRAY_BUFFER, gl_format );
		}
	}

	return nullptr;
}

gl_index_buffer::gl_index_buffer( unsigned gl_format )
	: _gl_format( gl_format )
{}