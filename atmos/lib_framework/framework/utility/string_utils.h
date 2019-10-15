
#pragma once

#include <cstdarg>
#include <string_view>
#include <vector>

namespace string_utils
{
	// c_hash(nullptr) == c_hash("") == seed * prime
	constexpr
		uint32_t c_hash_32(
			const char* str,
			uint32_t seed = 2166136261U,
			uint32_t i = 0)
	{
		uint64_t prime = 16777619U;
		return (str == nullptr || str[i] == 0)
			? uint32_t(seed * prime)
			: c_hash_32(str, uint32_t((seed ^ (uint32_t)str[i]) * prime), i + 1);
	}

	//------------------------------------------------------------------------------
	using const_ref = const std::string_view&;

	bool begins_with(const_ref string, const_ref begin);
	
	bool ends_with(const_ref string, const std::string& ending);

	std::string format_with_va_args(                     const char* fmt, const std::va_list&);

	void        format_with_va_args(std::string* result, const char* fmt, const std::va_list&);

	std::string format(                                  const char* fmt, ...);

	void        format(             std::string* result, const char* fmt, ...);

	void                     split(std::vector<std::string>* result,              const_ref in,     const_ref delim);

	std::vector<std::string> split(                                               const_ref in, const_ref delimiters);

	void                     split_single_delim(std::vector<std::string>* result, const_ref in, const char delimiter);

	std::vector<std::string> split_single_delim(                                  const_ref in, const char delimiter);

	void                     split_string_delim(std::vector<std::string>* result, const_ref in, const_ref delimiter);

	std::vector<std::string> split_with_escaped_quotes(                           const_ref in, const_ref delim);
}