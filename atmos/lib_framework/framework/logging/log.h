
#pragma once

#include "framework/meta_programming/va_macro.h"

#include <string_view>

#define LOG_CRITICAL( ... )          \
	logging::log_context(            \
		__FILE__,                    \
		__LINE__,                    \
		__FUNCSIG__,                 \
		logging::severity::CRITICAL, \
		__VA_ARGS__ );               \
	__debugbreak()
#define LOG_WARNING( ... )          \
	logging::log_context(           \
		__FILE__,                   \
		__LINE__,                   \
		__FUNCSIG__,                \
		logging::severity::WARNING, \
		__VA_ARGS__ );              \
	__debugbreak()
#define LOG_DEBUG( ... )          \
	logging::log_context(         \
		__FILE__,                 \
		__LINE__,                 \
		__FUNCSIG__,              \
		logging::severity::DEBUG, \
		__VA_ARGS__ )
#define LOG_VERBOSE( ... )          \
	logging::log_context(           \
		__FILE__,                   \
		__LINE__,                   \
		__FUNCSIG__,                \
		logging::severity::VERBOSE, \
		__VA_ARGS__ )
#define LOG_CONTEXT( SEVERITY, ... ) \
	logging::log_context(            \
		__FILE__,                    \
		__LINE__,                    \
		__FUNCSIG__,                 \
		SEVERITY,                    \
		__VA_ARGS__ )
#define EXCEPTION_CONTEXT( ... ) \
	logging::exception_string( __FILE__, __LINE__, __FUNCSIG__, __VA_ARGS__ )

#if ATMOS_DEBUG
#	define ASSERT_CONTEXT( ... )     \
		if ( false ==                 \
			 logging::assert_context( \
				 __FILE__,            \
				 __LINE__,            \
				 __FUNCSIG__,         \
				 #__VA_ARGS__,        \
				 __VA_ARGS__ ) )      \
			__debugbreak();

#	define ASSERT_CAST( T, VAL )                      \
		[&]( const char* func_sig ) {                  \
			auto val = VAL;                            \
			if ( false ==                              \
				 logging::assert_context(              \
					 __FILE__,                         \
					 __LINE__,                         \
					 func_sig,                         \
					 "",                               \
					 !val || dynamic_cast< T >( val ), \
					 "%s is not expected type: %s",    \
					 #VAL,                             \
					 #T ) )                            \
			{                                          \
				__debugbreak();                        \
			}                                          \
			return reinterpret_cast< T >( val );       \
		}( __FUNCSIG__ )

#else
#	define ASSERT_CONTEXT( ... ) ASSERT_CONTEXT_UNUSED_ARGS_( __VA_ARGS__ )
#	define ASSERT_CONTEXT_UNUSED_ARGS_( ... ) \
		VA_DISTRIBUTE_OP( ASSERT_CONTEXT_UNUSED_ARG_, __VA_ARGS__ )
#	define ASSERT_CONTEXT_UNUSED_ARG_( expr ) (void)( expr );

#	define ASSERT_CAST( T, VAL ) [&] { return reinterpret_cast< T >( VAL ); }()
#endif

namespace logging
{
	ENUM_SERIALIZABLE(

		severity,
		DEFAULT( UNDEFINED ),

		( UNDEFINED, 0 ),
		( CRITICAL, 1 ),
		( WARNING, 2 ),
		( DEBUG, 3 ),
		( VERBOSE, 4 ) );

	void log_context(
		const char* file,
		int line,
		const char* func,
		severity,
		const char* fmt,
		... );

	bool assert_context(
		const char* file,
		int line,
		const char* func,
		const char* expr,
		bool cond,
		const char* fmt,
		... );

	bool assert_context(
		const char* file,
		int line,
		const char* func,
		const char* expr,
		bool cond );

	std::string exception_string(
		const char* file,
		int line,
		const char* func,
		const char* fmt,
		... );
}
