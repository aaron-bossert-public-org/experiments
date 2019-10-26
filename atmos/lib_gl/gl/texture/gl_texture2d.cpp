
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

void gl_texture2d::map(size_t byte_size, buffer_view_base* out_mapped_view)
{
	_buffer.map(byte_size, out_mapped_view);
}

void gl_texture2d::unmap(
	const state& state)
{
	unmap(
		_buffer.mapped_view(),
		state);
}

void gl_texture2d::unmap()
{
	auto mapped_view = _buffer.mapped_view();
	gl_texture2d::state state = {};

	if (!mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		buffer_view<char> parsed_buffer_view = {};

		if (!texture_file_parsing::parse_as_ktx(mapped_view, &parsed_buffer_view, &state))
		{
			if (!texture_file_parsing::parse_as_dds(mapped_view, &parsed_buffer_view, &state))
			{
				if (!texture_file_parsing::parse_as_pvr(mapped_view, &parsed_buffer_view, &state))
				{
					texture_file_parsing::compressed_parser compressed_parser = mapped_view;
					if (compressed_parser.format != texture_format::UNDEFINED)
					{
						state.res = compressed_parser.res;
						state.format = compressed_parser.format;
						unmap(
							compressed_parser.decompressed, 
							state);
					}

					return;
				}
			}
		}

		unmap(
			parsed_buffer_view,
			state);
	}
}

size_t gl_texture2d::byte_size() const
{
	return _buffer.byte_size();
}

void gl_texture2d::unmap(
	const buffer_view<char>& texture_data,
	const state& state)
{
	GLenum internal_format = GL_FALSE;
	GLenum type = GL_FALSE;

	switch (state.format)
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
			_cfg.name.c_str(), igpu::to_string(state.format).data());
	}

	if (internal_format)
	{
		scoped_handle _(_gl_handle);

		_gpu_mem_metric.reset();
		_gpu_mem_metric.add(texture_data.size());

		// need to track down all the ways of calculating per mipmap byte offsets.
		// until then treat all textures as having one mipmap.
		// (the issue is that the highest mip levels use format dependent byte sizes, pvr for instance is clamped to a min of 4x4 texels)
		ASSERT_CONTEXT(state.mipmap_count == 1, "currently mipmaps not supported");
		if (type)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, state.res.x, state.res.y, 0, internal_format, type, texture_data.data());
			if (_cfg.can_auto_generate_mips && state.mipmap_count == 1)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		else
		{
			glCompressedTexImage2D(GL_TEXTURE_2D, 0, internal_format, state.res.x, state.res.y, 0, (GLsizei)texture_data.size(), texture_data.data());
		}

		_state = state;
	}

	_buffer.unmap();
}

const gl_texture2d::config& gl_texture2d::cfg() const
{
	return _cfg;
}

const texture2d::state& gl_texture2d::current_state() const
{
	return _state;
}

std::unique_ptr<gl_texture2d> gl_texture2d::make(const config& cfg)
{
	return std::unique_ptr<gl_texture2d>(
		new gl_texture2d(cfg));
}

gl_texture2d::gl_texture2d(
	const config& cfg)
	: _cfg(cfg)
	, _buffer(cfg)
	, _gpu_mem_metric(perf::category::GPU_MEM_USAGE, "Texture 2D Mem")
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