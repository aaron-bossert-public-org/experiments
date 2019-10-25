
#pragma once

#define ATMOS_USE_GL 1

#include <framework/logging/log.h>

#if ATMOS_BUILD_IOS
#	include <OpenGLES/ES3/gl.h>
#elif ATMOS_BUILD_ANDROID
#	include <GLES2/gl3.h>
#elif ATMOS_BUILD_WINDOWS
#	define GL_GLES_PROTOTYPES 1
#	define GL_GLEXT_PROTOTYPES 1
#	include <GLES3/gl3.h>
#	include <GLES2/gl2ext.h>
#elif ATMOS_BUILD_OSX
#	include <OpenGL/gl.h>
#	include <OpenGL/glu.h>
#else
#	error No build platform defined ATMOS_BUILD_IOS/ ATMOS_BUILD_WINDOWS/ etc...
#endif

#if ATMOS_DEBUG 
#	include <gl/defines/gl_debug.h>
#else
#   define CheckGlErrorLeaks()
#endif