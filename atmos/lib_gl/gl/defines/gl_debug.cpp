

#include <gl/defines/gl_includes.h>
#include <framework/logging/log.h>

gl_debug gl_debug::instance;

void gl_debug::set_errors_enabled(bool enabled, const char* reason, const char* func, const char* file, unsigned line)
{
	ASSERT_CONTEXT(enabled != _enabled);
    
    if(!enabled)
    {
		check_for_errors("Unknown origin, caught while caught while turning errors off\n\tReason for disabling errors:", reason, func, file, line, logging::severity::CRITICAL);
        
        _enabled        = enabled;
        _disabled_reason = reason;
        _disabled_in_func = func;
		_disabled_in_file = file;
        _disabled_in_line = line;
    }
    else
    {
        _enabled = true;
        
        if(false == check_for_errors("Unknown origin, occurred while errors were disabled", "", _disabled_in_func, _disabled_in_file, _disabled_in_line, logging::severity::CRITICAL))
        {
			logging::log_context(file, line, func, logging::severity::CRITICAL,
				"Info given when errors were disabled:\nReason stated:\n\t%s",
				_disabled_reason);
        }
    }
}

bool gl_debug::check_for_errors(const char* gl_func, const char* args_str, const char* func, const char* file, unsigned line, logging::severity severity)
{
    if(!_enabled)
        return true;
    
    bool is_good = true;
    
    GLenum code;
    while ((code = glGetError()) != GL_NO_ERROR)
    {
		is_good = false;

        std::string message;

#define CheckError(code) \
case code: \
	logging::log_context(file, line, func, severity, "%s : %s(%s)", #code, gl_func, args_str);\
	break;
        
        switch (code)
        {
                CheckError(                  GL_INVALID_ENUM)
                CheckError(                 GL_INVALID_VALUE)
                CheckError(             GL_INVALID_OPERATION)
                CheckError( GL_INVALID_FRAMEBUFFER_OPERATION)
                CheckError(                 GL_OUT_OF_MEMORY)
            default:
				logging::log_context(file, line, func, severity, "Unrecognized error(%d) : %s(%s)", code, gl_func, args_str);
				break;
        }
    }
    
    return is_good;
}
