
// #include <vulkan/texture/vulkan_depth_texture2d.h>

// // Vulkan implementation includes - begin
// #include <vulkan/defines/vulkan_includes.h>
// #include <vulkan/texture/vulkan_sampler.h>
// // Vulkan implementation includes - end

// using namespace igpu;

// void vulkan_depth_texture2d::attach()
// {
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gl_handle, 0);
// }

// GLuint vulkan_depth_texture2d::gl_handle() const
// {
//     return _gl_handle;
// }

// vulkan_depth_texture2d::~vulkan_depth_texture2d()
// {
//     glDeleteTextures(1, &_gl_handle);
// }

// std::unique_ptr<vulkan_depth_texture2d> vulkan_depth_texture2d::make(const config& cfg)
// {
// 	if (0 >= cfg.res.x || 0 >= cfg.res.y)
// 	{
// 		LOG_CRITICAL("width(%d) and height(%d) must be greater than zero", cfg.res.x, cfg.res.y);
// 		return nullptr;
// 	}

// 	GLint internal_format;
// 	GLenum format;
// 	GLenum type;

// 	switch (cfg.format)
// 	{
// 	case depth_format::D24S8:
// 		internal_format = GL_DEPTH24_STENCIL8;
// 		format = GL_DEPTH_STENCIL;
// 		type = GL_UNSIGNED_INT_24_8;
// 		break;
// 	case depth_format::D32:
// 		internal_format = GL_DEPTH_COMPONENT32F;
// 		format = GL_DEPTH_COMPONENT;
// 		type = GL_UNSIGNED_INT;
// 		break;
// 	case depth_format::D16:
// 		internal_format = GL_DEPTH_COMPONENT16;
// 		format = GL_DEPTH_COMPONENT;
// 		type = GL_UNSIGNED_SHORT;
// 		break;
// 	default:
// 		LOG_CRITICAL("texture(%s): no handler for format(%s)",
// 			cfg.name.c_str(), to_string(cfg.format).data());
// 		return nullptr;
// 	}
	
// 	GLuint gl_handle;
// 	GLint active_handle = 0;
// 	glGetIntegerv(GL_TEXTURE_BINDING_2D, &active_handle);
// 	glGenTextures(1, &gl_handle);
// 	glBindTexture(GL_TEXTURE_2D, gl_handle);

// 	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, cfg.res.x, cfg.res.y, 0, format, type, nullptr);

// 	// set texture min/mag 'nification, as well as wrapping and debug name
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_gl(cfg.sampler.min_filter));
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl(cfg.sampler.mag_filter));
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl(cfg.sampler.addressu));
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl(cfg.sampler.addressu));
// 	glLabelObjectEXT(GL_TEXTURE, gl_handle, (GLsizei)cfg.name.size(), cfg.name.c_str());

// 	glBindTexture(GL_TEXTURE_2D, active_handle);

// 	return std::unique_ptr<vulkan_depth_texture2d>(new vulkan_depth_texture2d(cfg, gl_handle));
// }

// vulkan_depth_texture2d::vulkan_depth_texture2d(const config& cfg, GLuint gl_handle)
// : depth_texture2d(cfg)
// , _gl_handle(gl_handle)
// , _gpu_mem_metric(perf::category::MEM_USAGE, "GPU Depth Texture Buffer Mem")
// {
// 	const depth_format format = cfg.format;
// 	_gpu_mem_metric.add(cfg.res.x * cfg.res.y *
// 		( format == depth_format::D24S8 ? 4
// 		: format == depth_format::D32 ? 4
// 		: format == depth_format::D16 ? 2
// 		: 0)); // should not hit
// }
