
#pragma once

#pragma warning( disable : 4068 )


#include "gl/defines/gl_redefine_functions.h"

#define RedefineGLFuncVoidReturn( GLFunc, ... )                \
	[&]( const char* func, const char* file, unsigned line ) { \
		GLFunc( __VA_ARGS__ );                                 \
		gl_debug::instance.check_for_errors(                   \
			#GLFunc,                                           \
			#__VA_ARGS__,                                      \
			func,                                              \
			file,                                              \
			line,                                              \
			logging::severity::CRITICAL );                     \
	}( __FUNCSIG__, __FILE__, __LINE__ )


#define RedefineGLFunc( GLFunc, ... )                          \
	[&]( const char* func, const char* file, unsigned line ) { \
		auto ret = GLFunc( __VA_ARGS__ );                      \
		gl_debug::instance.check_for_errors(                   \
			#GLFunc,                                           \
			#__VA_ARGS__,                                      \
			func,                                              \
			file,                                              \
			line,                                              \
			logging::severity::CRITICAL );                     \
		return ret;                                            \
	}( __FUNCSIG__, __FILE__, __LINE__ )

#undef glBegin
#undef glEnd
#undef glGetError

// Disable error checking between glBegin and glEnd
#define glBegin( ... )                                                         \
	gl_debug::instance.set_errors_enabled(                                     \
		false,                                                                 \
		"'glBegin' was called, cannot check for gl errors while in immediate " \
		"mode",                                                                \
		__FUNCSIG__,                                                           \
		__FILE__,                                                              \
		__LINE__ );                                                            \
	glBegin( __VA_ARGS__ )

#define glEnd( ... )      \
	glEnd( __VA_ARGS__ ); \
	gl_debug::instance    \
		.set_errors_enabled( true, "", __FUNCSIG__, __FILE__, __LINE__ )

// Libraries that do not process their own glerrors will have to be guarded to
// prevent those errors from leaking into first party code
#define CheckGlErrorLeaks()                                    \
	[&]( const char* func, const char* file, unsigned line ) { \
		gl_debug::instance.check_for_errors(                   \
			"glGetError",                                      \
			"checking third party gl errors",                  \
			func,                                              \
			file,                                              \
			line,                                              \
			logging::severity::CRITICAL );                     \
	}( __FUNCSIG__, __FILE__, __LINE__ )

// disable manual error checking, which would be redundant
#define glGetError( ... ) 0

class gl_debug
{
public:
	static gl_debug instance;

public:
	void set_errors_enabled(
		bool enabled,
		const char* reason,
		const char* func,
		const char* file,
		unsigned line );

	bool check_for_errors(
		const char* gl_func,
		const char* args_str,
		const char* func,
		const char* file,
		unsigned line,
		logging::severity );

private:
	bool _enabled = true;
	const char* _disabled_reason = "";
	const char* _disabled_in_func = "";
	const char* _disabled_in_file = "";
	unsigned _disabled_in_line = 0;
};
