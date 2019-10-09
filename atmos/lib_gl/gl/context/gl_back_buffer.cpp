
#include <gl/context/gl_back_buffer.h>

#include <framework/logging/log.h>

// GL implementation includes - begin
#include <gl/texture/gl_color_buffer.h>
#include <gl/texture/gl_depth_buffer.h>
// GL implementation includes - end

using namespace igpu;

const gl_back_buffer::config& gl_back_buffer::cfg() const
{
	return _cfg;
}

int gl_back_buffer::gl_handle() const
{
    return _gl_handle;
}

void gl_back_buffer::gl_handle(int gl_handle)
{
    _gl_handle = gl_handle;
}

std::shared_ptr<gl_back_buffer> gl_back_buffer::instance()
{
    // this lives as long as the process
	static auto *s_instance = new std::shared_ptr<gl_back_buffer>(
		gl_back_buffer::make({
			gl_color_buffer::make_empty(),
			gl_depth_buffer::make_empty()}));
    
    return *s_instance;
}

std::unique_ptr<gl_back_buffer> gl_back_buffer::make(
	const config& cfg)
{
    if (!cfg.color)
    {
        LOG_CONTEXT( CRITICAL, "cfg.color is null");
    }
    else if (!cfg.depth)
    {
        LOG_CONTEXT( CRITICAL, "cfg.depth is null");
    }
    else
    {
        return std::unique_ptr<gl_back_buffer>(new gl_back_buffer(cfg));
    }
    
    return nullptr;
}

gl_back_buffer::gl_back_buffer(const config& cfg)
: _cfg(cfg)
, _gl_handle(0)
{
}
