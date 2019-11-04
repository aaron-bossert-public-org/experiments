
#include "framework/logging/log.h"

#include "framework/meta_programming/va_macro.h"
#include "framework/utility/string_utils.h"

namespace
{
#if ATMOS_BUILD_WINDOWS
#	include <windows.h>
	void do_logging( const char* o )
	{
		OutputDebugString( o );
		OutputDebugString( "\n" );
	}
#endif

	std::string format(
		const char* file,
		int32_t line,
		const char* func,
		logging::severity s,
		const char* type,
		const char* msg )

	{
		if ( msg && *msg )
		{
			return string_utils::format(
				"%s(%d): %s %s\n\t%s\n\t%s\n",
				file,
				line,
				type,
				to_string( s ).data(),
				func,
				msg );
		}

		return string_utils::format(
			"%s(%d): %s %s\n\t%s\n",
			file,
			line,
			type,
			to_string( s ).data(),
			func );
	}
}	 // namespace

void logging::log_context(
	const char* file,
	int line,
	const char* func,
	severity s,
	const char* fmt,
	... )
{
	if ( s > severity::DEBUG )
	{
		return;
	}

	std::va_list args;
	va_start( args, fmt );
	std::string output = string_utils::format_with_va_args( fmt, args );
	va_end( args );

	output = format( file, line, func, s, "LOG", output.c_str() );

	do_logging( output.c_str() );
}

bool logging::assert_context(
	const char* file,
	int line,
	const char* func,
	const char*,
	bool cond,
	const char* fmt,
	... )
{
	if ( false == cond )
	{
		std::va_list args;
		va_start( args, fmt );
		std::string output = string_utils::format_with_va_args( fmt, args );
		va_end( args );

		assert_context( file, line, func, output.c_str(), cond );
	}

	return cond;
}

bool logging::assert_context(
	const char* file,
	int line,
	const char* func,
	const char* expr,
	bool cond )
{
	if ( false == cond )
	{
		std::string output =
			format( file, line, func, severity::CRITICAL, "ASSERT", expr );
		do_logging( output.c_str() );
	}

	return cond;
}

void test( const char* fmt, ... )
{
	std::va_list args;
	va_start( args, fmt );
	std::string output = string_utils::format_with_va_args( fmt, args );
	va_end( args );
}

std::string logging::exception_string(
	const char* file,
	int line,
	const char* func,
	const char* fmt,
	... )
{
	std::va_list args;
	va_start( args, fmt );
	std::string output = string_utils::format_with_va_args( fmt, args );
	va_end( args );

	output = format(
		file,
		line,
		func,
		severity::CRITICAL,
		"EXCEPTION",
		output.c_str() );

	return output;
}
