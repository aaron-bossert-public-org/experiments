//#include <vulkan/texture/vulkan_depth_buffer.h>
//
//// Vulkan implementation includes - begin
//#include <vulkan/defines/vulkan_includes.h>
//// Vulkan implementation includes - end
//
//
//using namespace igpu;
//
//void vulkan_depth_buffer::attach()
//{
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _gl_handle);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _gl_handle);
//}
//
//void vulkan_depth_buffer::gl_handle(int gl_handle)
//{
//	_gl_handle = gl_handle;
//}
//
//int vulkan_depth_buffer::gl_handle() const
//{
//	return _gl_handle;
//}
//
//vulkan_depth_buffer::~vulkan_depth_buffer()
//{
//	if (_gl_handle)
//	{
//		glDeleteRenderbuffers(1, (unsigned int*)&_gl_handle);
//	}
//}
//
//std::unique_ptr<vulkan_depth_buffer> vulkan_depth_buffer::make_empty()
//{
//	return std::unique_ptr<vulkan_depth_buffer>(new vulkan_depth_buffer({}, 0));
//}
//
//std::unique_ptr<vulkan_depth_buffer> vulkan_depth_buffer::make(const config& cfg)
//{
//	GLenum format;
//
//	switch (cfg.format)
//	{
//	case depth_format::D24S8:
//		format = GL_DEPTH24_STENCIL8;
//		break;
//	case depth_format::D32:
//		format = GL_DEPTH_COMPONENT32F;
//		break;
//	case depth_format::D16:
//		format = GL_DEPTH_COMPONENT16;
//		break;
//	default:
//		LOG_CONTEXT(CRITICAL, "texture(%s): no handler for format(%s)",
//			cfg.name.c_str(), to_string(cfg.format).data());
//		return nullptr;
//	}
//
//	GLuint gl_handle = 0;
//	GLint active_handle = 0;
//	glGetIntegerv(GL_RENDERBUFFER_BINDING, &active_handle);
//	glGenRenderbuffers(1, &gl_handle);
//	glBindRenderbuffer(GL_RENDERBUFFER, gl_handle);
//	glRenderbufferStorage(GL_RENDERBUFFER, format, cfg.res.x, cfg.res.y);
//	glBindRenderbuffer(GL_RENDERBUFFER, active_handle);
//
//	return std::unique_ptr<vulkan_depth_buffer>(new vulkan_depth_buffer(cfg, gl_handle));
//}
//
//vulkan_depth_buffer::vulkan_depth_buffer(const config& cfg, int gl_handle)
//	: depth_buffer(cfg)
//	, _gl_handle(gl_handle)
//	, _gpu_mem_metric(perf::category::MEM_USAGE, "GPU Depth Buffer Mem")
//{
//	const depth_format format = cfg.format;
//	_gpu_mem_metric.add(cfg.res.x * cfg.res.y *
//		( format == depth_format::D24S8 ? 4
//		: format == depth_format::D32 ? 4
//		: format == depth_format::D16 ? 2
//		: 0)); // should not hit
//}
