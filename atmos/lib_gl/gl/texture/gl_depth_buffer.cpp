#include <gl/texture/gl_depth_buffer.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
// GL implementation includes - end


using namespace igpu;

const gl_depth_buffer::config& gl_depth_buffer::cfg() const
{
	return _cfg;
}

void gl_depth_buffer::attach() const
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _gl_handle);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _gl_handle);
}

void gl_depth_buffer::gl_handle(int gl_handle)
{
    _gl_handle = gl_handle;
}

gl_depth_buffer::~gl_depth_buffer()
{
    if(_gl_handle)
    {
        glDeleteRenderbuffers(1, (unsigned int*)&_gl_handle);
    }
}

std::unique_ptr<gl_depth_buffer> gl_depth_buffer::make_empty()
{
    return std::unique_ptr<gl_depth_buffer>(new gl_depth_buffer({}, 0));
}

std::unique_ptr<gl_depth_buffer> gl_depth_buffer::make(const config& cfg)
{
	GLenum format;

	switch (cfg.format)
	{
	case depth_format::D24S8:
		format = GL_DEPTH24_STENCIL8;
		break;
	case depth_format::D32:
		format = GL_DEPTH_COMPONENT32F;
		break;
	case depth_format::D16:
		format = GL_DEPTH_COMPONENT16;
		break;
	default:
		LOG_CRITICAL("texture(%s): no handler for format(%s)",
			cfg.name.c_str(), to_string(cfg.format).data());
		return nullptr;
	}

    GLuint gl_handle = 0;
    GLint active_handle = 0;
    glGetIntegerv(GL_RENDERBUFFER_BINDING, &active_handle);
    glGenRenderbuffers(1, &gl_handle);
    glBindRenderbuffer(GL_RENDERBUFFER, gl_handle);
    glRenderbufferStorage(GL_RENDERBUFFER, format, cfg.res.x, cfg.res.y);
    glBindRenderbuffer(GL_RENDERBUFFER, active_handle);
    
    return std::unique_ptr<gl_depth_buffer>(new gl_depth_buffer(cfg, gl_handle));
}

gl_depth_buffer::gl_depth_buffer(const config& cfg, int gl_handle)
: _cfg(cfg)
, _gl_handle(gl_handle)
, _gpu_mem_metric(perf::category::GPU_MEM_USAGE, "GPU Depth Buffer Mem")
{
	const depth_format format = cfg.format;
	_gpu_mem_metric.add(cfg.res.x * cfg.res.y *
		( format == depth_format::D24S8 ? 4
		: format == depth_format::D32? 4
		: format == depth_format::D16? 2
		: 0)); // should not hit
}
