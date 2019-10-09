
#pragma once

#include <string_view>
#include <framework/meta_programming/va_macro.h>

#define LOG_CONTEXT(SEVERITY, ...) logging::log_context(__FILE__, __LINE__, __FUNCTION__, logging::severity::SEVERITY, __VA_ARGS__)
#define ASSERT_CONTEXT(...) if(false == logging::assert_context(__FILE__, __LINE__, #__VA_ARGS__, __VA_ARGS__)) logging::debug_break();

namespace logging {

	ENUM_SERIALIZABLE(

		severity, DEFAULT(CRITICAL),

		(CRITICAL, 0),
		(WARNING, 1),
		(DEBUG, 2),
		(INFORM, 3)
	);

	void log_context(const char* file, int line, const char* context, severity, const std::string_view& fmt, ...);

	bool assert_context(const char* file, int line, const char* context, bool cond, const std::string_view& fmt, ...);
	
	bool assert_context(const char* file, int line, const char* context, bool cond);

	void debug_break();
}
