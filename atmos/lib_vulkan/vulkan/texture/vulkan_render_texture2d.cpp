
// #include <vulkan/texture/vulkan_render_texture2d.h>

// // Vulkan implementation includes - begin
// #include <vulkan/defines/vulkan_includes.h>
// #include <vulkan/texture/vulkan_sampler.h>
// // Vulkan implementation includes - end

// using namespace igpu;

// void vulkan_render_texture2d::attach()
// {
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gl_handle, 0);
// }
    
// GLuint vulkan_render_texture2d::gl_handle() const
// {
//     return _gl_handle;
// }

// std::unique_ptr<vulkan_render_texture2d> vulkan_render_texture2d::make(const config& cfg)
// {
// 	if (0 >= cfg.res.x || 0 >= cfg.res.y)
// 	{
// 		LOG_CONTEXT(CRITICAL, "width(%d) and height(%d) must be greater than zero", cfg.res.x, cfg.res.y);
// 		return nullptr;
// 	}

// 	GLenum format;
// 	GLenum type;

// 	switch (cfg.format)
// 	{
// 	case color_format::INT_R8G8B8A8:
// 		format = GL_RGBA;
// 		type = GL_UNSIGNED_BYTE;
// 		break;
// 	case color_format::INT_R5G6B5:
// 		format = GL_RGB;
// 		type = GL_UNSIGNED_SHORT_5_6_5;
// 		break;
// 	case color_format::INT_R16:
// 		format = GL_RED;
// 		type = GL_UNSIGNED_SHORT;
// 		break;
// 	case color_format::FLOAT_R16:
// 		format = GL_RED;
// 		type = GL_HALF_FLOAT_OES;
// 		break;
// 	default:
// 		LOG_CONTEXT(CRITICAL, "texture(%s): no handler for format(%s)",
// 			cfg.name.c_str(), to_string(cfg.format).data());
// 		return nullptr;
// 	}

// 	GLuint gl_handle;
// 	GLint active_handle = 0;
// 	glGetIntegerv(GL_TEXTURE_BINDING_2D, &active_handle);
// 	glGenTextures(1, &gl_handle);
// 	glBindTexture(GL_TEXTURE_2D, gl_handle);
// 	const glm::ivec2& res = cfg.res;
	
// 	glTexImage2D(GL_TEXTURE_2D, 0, format, res.x, res.y, 0, format, type, nullptr);

// 	// set texture min/mag filters, wrapping, and debug name
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_gl(cfg.sampler.min_filter));
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl(cfg.sampler.mag_filter));
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl(cfg.sampler.addressu));
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl(cfg.sampler.addressu));
// 	glLabelObjectEXT(GL_TEXTURE, gl_handle, (GLsizei)cfg.name.size(), cfg.name.c_str());

// 	glBindTexture(GL_TEXTURE_2D, active_handle);

// 	return std::unique_ptr<vulkan_render_texture2d>(new vulkan_render_texture2d(cfg, gl_handle));
// }

// vulkan_render_texture2d::~vulkan_render_texture2d()
// {
//     glDeleteTextures(1, &_gl_handle);
// }
    
// vulkan_render_texture2d::vulkan_render_texture2d(const config& cfg, GLuint gl_handle)
// : render_texture2d(cfg)
// , _gl_handle(gl_handle)
// , _gpu_mem_metric(perf::category::MEM_USAGE, "GPU Render Texture Buffer Mem")
// {
// 	const color_format format = cfg.format;
// 	_gpu_mem_metric.add( cfg.res.x * cfg.res.y *
//         ( format == color_format::INT_R8G8B8A8 ? 4.f
//         : format == color_format::INT_R5G6B5   ? 2.f
//         : format == color_format::INT_R16      ? 2.f
//         : format == color_format::FLOAT_R16    ? 2.f
//         : 0.f )); // should not hit
// }
