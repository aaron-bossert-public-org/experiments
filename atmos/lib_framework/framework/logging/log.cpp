
#include <framework/meta_programming/va_macro.h>

#include <framework/logging/log.h>
#include <framework/utility/string_utils.h>

namespace {
#if ATMOS_BUILD_WINDOWS
#include <windows.h>
	void do_logging(const std::string_view& o)
	{
		OutputDebugString(o.data());
		OutputDebugString("\n");
	}
#endif

	std::string format(
		const char* file,
		int line,
		const char* func,
		logging::severity s,
		const char* type,
		const std::string_view& msg)

	{
		const char* fmt = msg.size()
			? "%s(%d): %s %s\n\t%s\n%s\n"
			: "%s(%d): %s %s\n\t%s\n";

		return string_utils::format(
			fmt,
			file,
			line,
			type,
			to_string(s),
			func,
			msg.data());
	}
}

void logging::log_context(const char* file, int line, const char* func, severity s, const std::string_view& fmt, ...)
{
	const char* fmt_cstr = fmt.data();

	std::va_list args;
	va_start(args, fmt_cstr);
	std::string output = string_utils::format_with_va_args(fmt_cstr, args);
	va_end(args);

	output = format(file, line, func, s, "LOG", output);

	do_logging(output);
}

bool logging::assert_context(const char* file, int line, const char* func, const char*, bool cond, const std::string_view& fmt, ...)
{
	if (false == cond)
	{
		const char* fmt_cstr = fmt.data();

		std::va_list args;
		va_start(args, fmt_cstr);
		std::string output = string_utils::format_with_va_args(fmt_cstr, args);
		va_end(args);

		assert_context(file, line, func, output.c_str(), cond);
	}

	return cond;
}

bool logging::assert_context(const char* file, int line, const char* func, const char* expr, bool cond)
{
	if (false == cond)
	{
		std::string output = format(file, line, func, severity::CRITICAL, "ASSERT", expr);
		do_logging(output.c_str());
	}

	return cond;
}

std::string logging::exception_string(const char* file, int line, const char* func, const std::string_view& fmt, ...)
{
	const char* fmt_cstr = fmt.data();

	std::va_list args;
	va_start(args, fmt_cstr);
	std::string output = string_utils::format_with_va_args(fmt_cstr, args);
	va_end(args);

	output = format(file, line, func, severity::CRITICAL, "EXCEPTION", output);

	return output;
}

void logging::debug_break()
{
	DebugBreak();
}

