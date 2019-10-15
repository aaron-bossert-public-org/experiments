#include <framework/utility/string_utils.h>
#include <framework/logging/log.h>
#include <stdarg.h>
#include <bitset>

using namespace string_utils;

#pragma once

bool string_utils::begins_with(const_ref string, const_ref begin)
{
    if (string.length() >= begin.length())
    {
        return string.compare(0, begin.length(), begin) == 0;
    }
    else
    {
        return false;
    }
}

bool ends_with(const_ref string, const std::string& ending)
{
	if (string.length() >= ending.length())
	{
		return string.compare(string.length() - ending.length(), ending.length(), ending) == 0;
	}
	else
	{
		return false;
	}
}

void string_utils::format_with_va_args(std::string* result, const char* fmt, const std::va_list& args)
{
	va_list args1;
	va_copy(args1, args);
	result->resize(1 + vsnprintf(NULL, 0, fmt, args1));
	va_end(args1);

	va_list args2;
	va_copy(args2, args);
	vsnprintf(result->data(), result->size(), fmt, args2);
	va_end(args2);
}

std::string string_utils::format_with_va_args(const char* fmt, const std::va_list& args)
{
	std::string result;
	format_with_va_args(&result, fmt, args);
	return result;
}

std::string string_utils::format(const char* fmt, ...)
{
	std::va_list args;
	va_start(args, fmt);
	std::string result = string_utils::format_with_va_args(fmt, args);
	va_end(args);

	return result;
}

void string_utils::format(std::string* result, const char* fmt, ...)
{
	std::va_list args;
	va_start(args, fmt);
	format_with_va_args(result, fmt, args);
	va_end(args);
}

template<typename C>
void string_split_internal(const_ref s, const char* d, C& ret)
{
    C output;

    if(d == nullptr)
        return;

	std::bitset<255> delims;
    while( *d )
    {
        unsigned char code = *d++;
        delims[code] = true;
    }
    typedef std::string_view::const_iterator iter;
    iter beg;
    bool in_token = false;
    for( std::string_view::const_iterator it = s.begin(), end = s.end();
        it != end; ++it )
    {
        if( delims[(unsigned char)(*it)] )
        {
            if( in_token )
            {
                output.push_back(typename C::value_type(beg, it));
                in_token = false;
            }
        }
        else if( !in_token )
        {
            beg = it;
            in_token = true;
        }
    }
    if( in_token )
        output.push_back(typename C::value_type(beg, s.end()));
    output.swap(ret);
}

template<typename C>
void string_split_singledelim_internal(const_ref s, const char d, C& ret)
{
    C output;

    typedef std::string_view::const_iterator iter;
    iter beg;
    bool in_token = false;
    for( std::string_view::const_iterator it = s.begin(), end = s.end();
        it != end; ++it )
    {
        if( *it == d )
        {
            if( in_token )
            {
                output.push_back(typename C::value_type(beg, it));
                in_token = false;
            }
        }
        else if( !in_token )
        {
            beg = it;
            in_token = true;
        }
    }
    if( in_token )
        output.push_back(typename C::value_type(beg, s.end()));
    output.swap(ret);
}


void string_utils::split(std::vector<std::string>* result, const_ref in, const_ref delim)
{
    ASSERT_CONTEXT(result != nullptr);
    string_split_internal(in, delim.data(), *result);
}


std::vector<std::string> string_utils::split(const_ref in, const_ref delimiters)
{
    std::vector<std::string> out;
    string_split_internal(in, delimiters.data(), out);
    return out;
}

void string_utils::split_single_delim(std::vector<std::string>* result, const_ref in, const char delimiter)
{
	ASSERT_CONTEXT(result != nullptr);
    string_split_singledelim_internal(in, delimiter, *result);
}

std::vector<std::string> string_utils::split_single_delim(const_ref in, const char delimiter)
{
    std::vector<std::string> out;
    string_split_singledelim_internal(in, delimiter, out);
    return out;
}

void string_utils::split_string_delim(std::vector<std::string>* result, const_ref in, const_ref delimiter)
{
    size_t i=0;
	size_t start=0;
    const size_t length = in.length();
    while( start<length ) {
        while( i < length )
        {
            if(in[i] == delimiter[0])
            {
                int delimit_check_index = 0;
                for(delimit_check_index = 0; (delimit_check_index < (int)delimiter.length()) && (i + delimit_check_index < (int)in.length()); ++delimit_check_index)
                {
                    if(in[i + delimit_check_index] != delimiter[delimit_check_index])
                        break;
                }
                if(delimit_check_index == delimiter.length())
                {
                    break;
                }
            }
            ++i;
        }

        if(i-start>0)
            result->emplace_back((std::string)in.substr(start, i-start));

        i += delimiter.length();
        start = i;
    }
}

std::vector<std::string> string_utils::split_with_escaped_quotes(const_ref in, const_ref delim)
{
    const size_t str_length = in.length();
    size_t size_to_reserve   = str_length;

    if (size_to_reserve > 256)
        size_to_reserve = 256;

    std::string word;
    word.reserve(size_to_reserve);


    bool can_split = true;
    std::vector<std::string> out;

    for(size_t i = 0u; i < str_length; ++i)
    {
        bool splitting = false;
        for(size_t j = 0u; j < delim.length() && !splitting; ++j)
            splitting = (in[i] == delim[j]);

        if (in[i] == '"')
        {
            can_split = !can_split;
        }

        if (splitting && can_split)
        {
            if (word.length())
            {
                out.push_back(word);
                word.clear();
            }
        }
        else
        {
            word += in[i];
        }
    }

    if (!word.empty())
    {
        out.push_back(word);
    }

    return out;
}