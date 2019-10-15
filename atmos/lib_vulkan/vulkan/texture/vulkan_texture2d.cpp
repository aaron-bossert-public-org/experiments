//
//#include <vulkan/texture/vulkan_texture2d.h>
//
//// Vulkan implementation includes - begin
//#include <vulkan/defines/vulkan_includes.h>
//#include <vulkan/texture/vulkan_sampler.h>
//// Vulkan implementation includes - end
//
//#include <igpu/texture/texel_buffer2d.h>
//
//#include <framework/logging/log.h>
//
//using namespace igpu;
//
//
//std::unique_ptr<vulkan_texture2d> vulkan_texture2d::make(const texel_buffer2d& raw)
//{
//	GLuint gl_handle;
//	GLint active_handle = 0;
//	const auto& cfg = raw.cfg();
//
//	GLenum internal_format = GL_FALSE;
//	GLenum type = GL_FALSE;
//
//	switch (cfg.format)
//	{
//	case texture_format::INT_R8:
//		internal_format = GL_RED;
//		type = GL_UNSIGNED_BYTE;
//		break;
//	case texture_format::INT_R8G8:
//		internal_format = GL_RG_EXT;
//		type = GL_UNSIGNED_BYTE;
//		break;
//	case texture_format::INT_R8G8B8:
//		internal_format = GL_RGB;
//		type = GL_UNSIGNED_BYTE;
//		break;
//	case texture_format::INT_R8G8B8A8:
//		internal_format = GL_RGBA;
//		type = GL_UNSIGNED_BYTE;
//		break;
//
//	case texture_format::INT_R16:
//		internal_format = GL_RED;
//		type = GL_UNSIGNED_SHORT;
//		break;
//	case texture_format::INT_R16G16:
//		internal_format = GL_RG_EXT;
//		type = GL_UNSIGNED_SHORT;
//		break;
//	case texture_format::INT_R16G16B16:
//		internal_format = GL_RGB;
//		type = GL_UNSIGNED_SHORT;
//		break;
//	case texture_format::INT_R16G16B16A16:
//		internal_format = GL_RGBA;
//		type = GL_UNSIGNED_SHORT;
//		break;
//
//	case texture_format::PVR1_2BPP_RGB:
//		internal_format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
//		break;
//	case texture_format::PVR1_2BPP_RGBA:
//		internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
//		break;
//	case texture_format::PVR2_2BPP:
//		internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
//		break;
//
//	case texture_format::PVR1_4BPP_RGB:
//		internal_format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
//		break;
//	case texture_format::PVR1_4BPP_RGBA:
//		internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
//		break;
//	case texture_format::PVR2_4BPP:
//		internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
//		break;
//
//	case texture_format::ETC1_RGB:
//		internal_format = GL_ETC1_RGB8_OES;
//		break;
//
//	case texture_format::DXT_1_RGBA:
//		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
//		break;
//	case texture_format::DXT_3_RGBA:
//		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
//		break;
//	case texture_format::DXT_5_RGBA:
//		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
//		break;
//	default:
//
//		LOG_CONTEXT(CRITICAL, "texture(%s): unhandled format(%s)",
//			cfg.name.c_str(), to_string(cfg.format).data());
//		return nullptr;
//	}
//
//	glGetIntegerv(GL_TEXTURE_BINDING_2D, &active_handle);
//	glGenTextures(1, &gl_handle);
//	glBindTexture(GL_TEXTURE_2D, gl_handle);
//
//	if (type)
//	{
//		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, cfg.res.x, cfg.res.y, 0, internal_format, type, raw.texture_data().data());
//	}
//	else
//	{
//		glCompressedTexImage2D(GL_TEXTURE_2D, 0, internal_format, cfg.res.x, cfg.res.y, 0, (GLsizei)raw.texture_data().size(), raw.texture_data().data());
//	}
//
//	// these values will only be set if they have changed
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_gl(cfg.sampler.min_filter));
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl(cfg.sampler.mag_filter));
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl(cfg.sampler.addressu));
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl(cfg.sampler.addressu));
//	glLabelObjectEXT(GL_TEXTURE, gl_handle, (GLsizei)cfg.name.size(), cfg.name.c_str());
//
//	glBindTexture(GL_TEXTURE_2D, active_handle);
//
//
//	return std::unique_ptr<vulkan_texture2d>(new vulkan_texture2d(
//		cfg,
//		raw.texture_data().size(),
//		gl_handle));
//}
//
//vulkan_texture2d::vulkan_texture2d(
//	const texel_buffer2d::config& cfg,
//	size_t size,
//	unsigned gl_handle)
//	: texture2d(cfg, size)
//	, _gl_handle(gl_handle)
//	, _gpu_mem_metric(perf::category::MEM_USAGE, "GPU Texture 2D Mem")
//{
//	_gpu_mem_metric.add(size);
//}
//
//vulkan_texture2d::~vulkan_texture2d()
//{
//	if (_gl_handle)
//	{
//		glDeleteTextures(1, &_gl_handle);
//	}
//}
