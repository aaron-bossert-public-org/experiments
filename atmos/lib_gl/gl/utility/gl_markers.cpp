
#include "gl/utility/gl_markers.h"

#include "gl/defines/gl_includes.h"

using namespace igpu;

#if !defined( GL_EXT_debug_marker ) || !defined( GL_GLEXT_PROTOTYPES )

void gl_event_marker( const char* message, int length )
{}

void gl_push_marker( const char* message, int length )
{}

void gl_pop_marker()
{}

#else

void gl_event_marker( const char* message, size_t length )
{
	glInsertEventMarkerEXT( (GLsizei)length, message );
}

void gl_push_marker( const char* message, int length )
{
	glPushGroupMarkerEXT( (GLsizei)length, message );
}

void gl_pop_marker()
{
	glPopGroupMarkerEXT();
}

#endif
