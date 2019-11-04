//#include "gl/texture/gl_render_buffer.h"
//
//#include "gl/defines/gl_includes.h"
//
//
//using namespace igpu;
//
//const gl_render_buffer::config& gl_render_buffer::cfg() const
//{
//	return _cfg;
//}
//
//void gl_render_buffer::attach() const
//{
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _gl_handle);
//}
//
//void gl_render_buffer::gl_handle(int gl_handle)
//{
//    _gl_handle = gl_handle;
//}
//
//gl_render_buffer::~gl_render_buffer()
//{
//    if(_gl_handle)
//    {
//        glDeleteRenderbuffers(1, (unsigned int*)&_gl_handle);
//    }
//}
//
//std::unique_ptr<gl_render_buffer> gl_render_buffer::make_empty()
//{
//    return std::unique_ptr<gl_render_buffer>(new gl_render_buffer({}, 0));
//}
//
//std::unique_ptr<gl_render_buffer> gl_render_buffer::make(const config& cfg)
//{
//	GLenum format;
//
//	switch (cfg.format)
//	{
//#if defined(GL_RGBA8)
//	case color_format::INT_R8G8B8A8:
//		format = GL_RGBA8;
//		break;
//#elif defined(GL_RGBA8_OES)
//	case color_format::INT_R8G8B8A8:
//		format = GL_RGBA8_OES;
//		break;
//#endif
//	case color_format::INT_R5G6B5:
//		format = GL_RGB565;
//		break;
//#if defined(GL_R16I)
//	case color_format::INT_R16:
//		format = GL_R16I;
//		break;
//#endif
//	default:
//		LOG_CRITICAL("texture(%s): no handler for format(%s)",
//			cfg.name.c_str(), to_string(cfg.format).data());
//		return nullptr;
//	}
//
//    GLuint gl_handle = 0;
//    GLint active_handle = 0;
//    glGetIntegerv(GL_RENDERBUFFER_BINDING, &active_handle);
//    glGenRenderbuffers(1, &gl_handle);
//    glBindRenderbuffer(GL_RENDERBUFFER, gl_handle);
//    glRenderbufferStorage(GL_RENDERBUFFER, format, cfg.res.x, cfg.res.y);
//    glBindRenderbuffer(GL_RENDERBUFFER, active_handle);
//    
//    return std::unique_ptr<gl_render_buffer>(new gl_render_buffer(cfg, gl_handle));
//}
//
//gl_render_buffer::gl_render_buffer(const config& cfg, int gl_handle)
//: _cfg(cfg)
//, _gl_handle(gl_handle)
//, _gpu_mem_metric(perf::category::GPU_MEM_USAGE, "GPU Color Buffer Mem")
//{
//	const color_format format = cfg.format;
//	_gpu_mem_metric.add(cfg.res.x * cfg.res.y *
//		( format == color_format::INT_R8G8B8A8 ? 4
//		: format == color_format::INT_R5G6B5 ? 2
//		: format == color_format::INT_R16 ? 2
//		: format == color_format::FLOAT_R16 ? 2
//		: 0)); // should not hit
//}
