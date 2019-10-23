
#include <gl/texture/gl_texture2d.h>

// GL implementation includes - begin
#include <gl/defines/gl_includes.h>
#include <gl/texture/gl_sampler.h>
// GL implementation includes - end

#include <igpu/texture/texture_file_parsing.h>

#include <framework/logging/log.h>

using namespace igpu;

namespace
{
	struct scoped_handle
	{
		GLint active_handle = 0;
		scoped_handle(GLint handle)
		{
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &active_handle);
			glBindTexture(GL_TEXTURE_2D, handle);
		}

		~scoped_handle()
		{
			glBindTexture(GL_TEXTURE_2D, active_handle);
		}
	};
}

unsigned gl_texture2d::gl_handle() const
{
	return _gl_handle;
}

void gl_texture2d::map(size_t byte_size, buffer_view_base* out_texture_data)
{
	if (!_state.mapped_view.data() || _state.mapped_view.size() < byte_size)
	{
		_cpu_mem_metric.reset();
		_cpu_mem_metric.add(byte_size);
		_state = {};
		_state.mapped.reset(new char[byte_size]);
		_state.mapped_view = buffer_view<char>(byte_size, _state.mapped.get());
	}

	if (!_state.mapped_view.data())
	{
		LOG_CRITICAL("failed to create mapped buffer");
		*out_texture_data = buffer_view_base(
			0,
			nullptr,
			out_texture_data->stride());
	}
	else
	{
		*out_texture_data = buffer_view_base(
			byte_size / out_texture_data->stride(),
			_state.mapped.get(),
			out_texture_data->stride());
	}
}

void gl_texture2d::unmap_explicit(
	const glm::ivec2& res,
	texture_format format)
{
	if (!_state.mapped_view.size())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		_state.source = source_type::EXPLICIT;
		upload(_state.mapped_view, res, format);
	}
}

void gl_texture2d::unmap_raw_file()
{
	if (!_state.mapped_view.size())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		_state.source = source_type::RAW_FILE;
		buffer_view<char> parsed_buffer_view = {};
		glm::ivec2 res = {};
		texture_format format = {};
		size_t mipmap_count = 0;

		if (!texture_file_parsing::parse_as_ktx(_state.mapped_view, &parsed_buffer_view, &res, &format, &mipmap_count))
		{
			if (!texture_file_parsing::parse_as_dds(_state.mapped_view, &parsed_buffer_view, &res, &format, &mipmap_count))
			{
				if (!texture_file_parsing::parse_as_pvr(_state.mapped_view, &parsed_buffer_view, &res, &format, &mipmap_count))
				{
					texture_file_parsing::compressed_parser compressed_parser = _state.mapped_view;
					if (compressed_parser.format != texture_format::UNDEFINED)
					{
						upload(compressed_parser.decompressed, compressed_parser.res, compressed_parser.format);
					}

					return;
				}
			}
		}

		upload(parsed_buffer_view, res, format);
	}
}

void gl_texture2d::upload(
	const buffer_view<char>& texture_data,
	const glm::ivec2& res,
	texture_format format)
{
	GLenum internal_format = GL_FALSE;
	GLenum type = GL_FALSE;

	switch (format)
	{
	case texture_format::INT_R8:
		internal_format = GL_RED;
		type = GL_UNSIGNED_BYTE;
		break;
	case texture_format::INT_R8G8:
		internal_format = GL_RG_EXT;
		type = GL_UNSIGNED_BYTE;
		break;
	case texture_format::INT_R8G8B8:
		internal_format = GL_RGB;
		type = GL_UNSIGNED_BYTE;
		break;
	case texture_format::INT_R8G8B8A8:
		internal_format = GL_RGBA;
		type = GL_UNSIGNED_BYTE;
		break;

	case texture_format::INT_R16:
		internal_format = GL_RED;
		type = GL_UNSIGNED_SHORT;
		break;
	case texture_format::INT_R16G16:
		internal_format = GL_RG_EXT;
		type = GL_UNSIGNED_SHORT;
		break;
	case texture_format::INT_R16G16B16:
		internal_format = GL_RGB;
		type = GL_UNSIGNED_SHORT;
		break;
	case texture_format::INT_R16G16B16A16:
		internal_format = GL_RGBA;
		type = GL_UNSIGNED_SHORT;
		break;

	case texture_format::PVR1_2BPP_RGB:
		internal_format = GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
		break;
	case texture_format::PVR1_2BPP_RGBA:
		internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
		break;
	case texture_format::PVR2_2BPP:
		internal_format = GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
		break;

	case texture_format::PVR1_4BPP_RGB:
		internal_format = GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
		break;
	case texture_format::PVR1_4BPP_RGBA:
		internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
		break;
	case texture_format::PVR2_4BPP:
		internal_format = GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;
		break;

	case texture_format::ETC1_RGB:
		internal_format = GL_ETC1_RGB8_OES;
		break;

	case texture_format::DXT_1_RGBA:
		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case texture_format::DXT_3_RGBA:
		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case texture_format::DXT_5_RGBA:
		internal_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:

		LOG_CRITICAL("texture(%s): unhandled format(%s)",
			_cfg.name.c_str(), igpu::to_string(format).data());
	}

	if (internal_format)
	{
		scoped_handle _(_gl_handle);

		_gpu_mem_metric.reset();
		_gpu_mem_metric.add(texture_data.size());

		if (type)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, res.x, res.y, 0, internal_format, type, texture_data.data());
			if (_cfg.can_auto_generate_mips)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		else
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, internal_format, res.x, res.y, 0, (GLsizei)texture_data.size(), texture_data.data());
		}

		_state = {
			_state.source,
			res,
			format,
			_state.mapped_view,
			std::move(_state.mapped)
		};
	}

	if (_cfg.usage == buffer_usage::STATIC)
	{
		_cpu_mem_metric.reset();
		_state = {
			_state.source,
			_state.res,
			_state.format,
			{},
			nullptr
		};
	}
}

const gl_texture2d::config& gl_texture2d::cfg() const
{
	return _cfg;
}

gl_texture2d::source_type gl_texture2d::source() const
{
	return _state.source;
}

const glm::ivec2& gl_texture2d::res() const
{
	return _state.res;
}

texture_format gl_texture2d::format() const
{
	return _state.format;
}

std::unique_ptr<gl_texture2d> gl_texture2d::make(const config& cfg)
{
	return std::unique_ptr<gl_texture2d>(
		new gl_texture2d(cfg));
}

gl_texture2d::gl_texture2d(
	const config& cfg)
	: _cfg(cfg)
	, _gpu_mem_metric(perf::category::GPU_MEM_USAGE, "Texture 2D Mem")
	, _cpu_mem_metric(perf::category::CPU_MEM_USAGE, "Texture 2D Mem")
	, _gl_handle(
		[] {
			GLuint handle = 0; 
			glGenTextures(1, &handle);
			return handle; 
		}())
{
	scoped_handle _(_gl_handle);
	
	// these values will only be set if they have changed
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, to_gl(_cfg.sampler.min_filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, to_gl(_cfg.sampler.mag_filter));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, to_gl(_cfg.sampler.addressu));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, to_gl(_cfg.sampler.addressu));
	glLabelObjectEXT(GL_TEXTURE, _gl_handle, (GLsizei)_cfg.name.size(), _cfg.name.c_str());
}