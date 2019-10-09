
#include <framework/meta_programming/va_macro.h>

#include <framework/logging/log.h>
#include <framework/utility/string_utils.h>

#if ATMOS_BUILD_WINDOWS
	#include <windows.h>
	namespace {
		void do_logging(const std::string_view& o)
		{
			OutputDebugString(o.data());
			OutputDebugString("\n");
		}
	}
#endif

void logging::log_context(const char* file, int line, const char* context, severity s, const std::string_view& fmt, ...)
{
	std::string output = string_utils::format("%s(%d): LOG %s\n\t%s\n%s\n", file, line, to_string(s), context, fmt.data());

	std::va_list args;
	const char* fmt_cstr = fmt.data();
	va_start(args, fmt_cstr);
	string_utils::format_with_va_args(&output, fmt, args);
	va_end(args);

	do_logging(output.c_str());
}

bool logging::assert_context(const char* file, int line, const char*, bool cond, const std::string_view& fmt, ...)
{
	if (false == cond)
	{
		std::va_list args;
		const char* fmt_cstr = fmt.data();
		va_start(args, fmt_cstr);
		auto context = string_utils::format_with_va_args(fmt, args);
		va_end(args);

		assert_context(file, line, context.c_str(), cond);
	}

	return cond;
}

bool logging::assert_context(const char* file, int line, const char* context, bool cond)
{
	if (false == cond)
	{
		std::string output = string_utils::format("%s(%d): ASSERT %s\n\t%s\n%s\n", file, line, context);
		do_logging(output.c_str());
	}

	return cond;
}

void logging::debug_break()
{
	DebugBreak();
}

