
#pragma once

#include <string_view>
#include <framework/meta_programming/va_macro.h>

#define LOG_CRITICAL(...) logging::log_context(__FILE__, __LINE__, __FUNCSIG__, logging::severity::CRITICAL, __VA_ARGS__)
#define LOG_WARNING(...) logging::log_context(__FILE__, __LINE__, __FUNCSIG__, logging::severity::WARNING, __VA_ARGS__)
#define LOG_DEBUG(...) logging::log_context(__FILE__, __LINE__, __FUNCSIG__, logging::severity::DEBUG, __VA_ARGS__)
#define LOG_VERBOSE(...) logging::log_context(__FILE__, __LINE__, __FUNCSIG__, logging::severity::VERBOSE, __VA_ARGS__)
#define LOG_CONTEXT(SEVERITY, ...) logging::log_context(__FILE__, __LINE__, __FUNCSIG__, SEVERITY, __VA_ARGS__)
#define ASSERT_CONTEXT(...) if(false == logging::assert_context(__FILE__, __LINE__, __FUNCSIG__, #__VA_ARGS__, __VA_ARGS__)) __debugbreak();
#define EXCEPTION_CONTEXT(...) logging::exception_string(__FILE__, __LINE__, __FUNCSIG__, __VA_ARGS__)

namespace logging {

	ENUM_SERIALIZABLE(

		severity, DEFAULT(CRITICAL),

		(CRITICAL, 0),
		(WARNING, 1),
		(DEBUG, 2),
		(VERBOSE, 3)
	);
	
	void log_context(const char* file, int line, const char* func, severity, const char* fmt, ...);

	bool assert_context(const char* file, int line, const char* func, const char* expr, bool cond, const char* fmt, ...);
	
	bool assert_context(const char* file, int line, const char* func, const char* expr, bool cond);
	
	std::string exception_string(const char* file, int line, const char* func, const char* fmt, ...);
}
