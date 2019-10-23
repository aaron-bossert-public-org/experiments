
#include <vulkan/texture/vulkan_staged_texture2d.h>

#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/buffer/vulkan_buffer_mediator.h>
#include <vulkan/texture/vulkan_image.h>

#include <igpu/texture/texture_file_parsing.h>

#include <framework/logging/log.h>

using namespace igpu;



vulkan_staged_texture2d::vulkan_staged_texture2d(
	const config& cfg,
	const scoped_ptr < vulkan_buffer_mediator >& buffer_mediator)
	: _cfg(cfg)
	, _buffer_mediator(buffer_mediator)
{
}

vulkan_staged_texture2d::~vulkan_staged_texture2d()
{
}

void vulkan_staged_texture2d::map(size_t byte_size, buffer_view_base* out_buffer_view)
{
	if (!_state.staging_buffer || _state.staging_buffer->cfg().size < byte_size)
	{
		_state.staging_buffer = vulkan_buffer::make({
			_buffer_mediator->vma(),
			VMA_MEMORY_USAGE_CPU_ONLY,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			byte_size,
			});
	}

	if (_state.staging_buffer)
	{
		_state.mapped_view = *out_buffer_view = buffer_view_base(
			byte_size / out_buffer_view->stride(),
			_state.staging_buffer->map(),
			out_buffer_view->stride());
	}
	else
	{
		LOG_CRITICAL("failed to create staging buffer");

		_state.mapped_view = *out_buffer_view = buffer_view_base(
			0,
			nullptr,
			out_buffer_view->stride());
	}
}

void vulkan_staged_texture2d::unmap_explicit(
	const glm::ivec2& res,
	texture_format format)
{
	if (!_state.mapped_view.size())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else if (!_state.staging_buffer)
	{
		LOG_CRITICAL("staging buffer is null");
	}
	else
	{
		_state.source = source_type::EXPLICIT;
		unmap(_state.mapped_view, res, format, 1);
	}
}

void vulkan_staged_texture2d::unmap_raw_file()
{
	if (!_state.mapped_view.size())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else if (!_state.staging_buffer)
	{
		LOG_CRITICAL("staging buffer is null");
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

					_state.staging_buffer->unmap();

					if (compressed_parser.format != texture_format::UNDEFINED)
					{
						buffer_view<char> mapped;
						map(compressed_parser.decompressed.size(), &mapped);
						memcpy(
							mapped.data(),
							compressed_parser.decompressed.data(),
							compressed_parser.decompressed.size());

						unmap(mapped, compressed_parser.res, compressed_parser.format, 1);
					}

					return;
				}
			}
		}

		unmap(parsed_buffer_view, res, format, mipmap_count);
	}
}

void vulkan_staged_texture2d::unmap(
	const buffer_view<char>& texture_data,
	const glm::ivec2& res,
	texture_format format,
	size_t mipmap_count)
{
	if (!_state.staging_buffer)
	{
		LOG_CRITICAL("staging_buffer is null");
		return;
	}

	bool generate_mipmaps = false;
	VkFormat vulkan_format = to_vulkan_format(format);

	_state.staging_buffer->unmap();

	if (vulkan_format)
	{
		auto& buffer_mediator = *_buffer_mediator;
		
		if (_cfg.can_auto_generate_mips && mipmap_count == 1)
		{
			if(buffer_mediator.can_generate_mipmaps(
				vulkan_format,
				VK_IMAGE_TILING_OPTIMAL))
			{
				generate_mipmaps = true;
				auto max_dim = res.x > res.y ? res.x : res.y;
				mipmap_count = static_cast<uint32_t>(std::log2(max_dim)) + 1;
			}
		}

		if (!_state.image ||
			_state.image->cfg().image_info.extent.width != (uint32_t)res.x ||
			_state.image->cfg().image_info.extent.height != (uint32_t)res.y ||
			_state.image->cfg().image_info.format != vulkan_format ||
			_state.image->cfg().image_info.mipLevels != mipmap_count)
		{
			VkImageCreateInfo image_info = {};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent = { (uint32_t)res.x, (uint32_t)res.y , 1 };
			image_info.mipLevels = (uint32_t)mipmap_count;
			image_info.arrayLayers = 1;
			image_info.format = vulkan_format;
			image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			image_info.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			image_info.samples = VK_SAMPLE_COUNT_1_BIT;
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;


			VkImageViewCreateInfo view_info = {};
			view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = vulkan_format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = image_info.mipLevels;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			VkSamplerCreateInfo sampler_info = {};
			sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler_info.magFilter = to_vulkan_filter(_cfg.sampler.mag_filter);
			sampler_info.minFilter = to_vulkan_filter(_cfg.sampler.min_filter);
			sampler_info.addressModeU = to_vulkan_address(_cfg.sampler.addressu);
			sampler_info.addressModeV = to_vulkan_address(_cfg.sampler.addressv);
			sampler_info.addressModeW = to_vulkan_address(sampler::address::DEFAULT);
			sampler_info.anisotropyEnable = VK_TRUE;
			sampler_info.maxAnisotropy = 16;
			sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampler_info.unnormalizedCoordinates = VK_FALSE;
			sampler_info.compareEnable = VK_FALSE;
			sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
			sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			sampler_info.minLod = 0;
			sampler_info.maxLod = (float)image_info.mipLevels;
			sampler_info.mipLodBias = 0;

			vulkan_image::config image_cfg = {};
			image_cfg.physical_device = buffer_mediator.cfg().physical_device;
			image_cfg.device = buffer_mediator.cfg().device;
			image_cfg.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			image_cfg.image_info = image_info;
			image_cfg.view_info = view_info;
			image_cfg.sampler_info = sampler_info;


			_state.image = nullptr;
			_state = {
				_state.source,
				{},
				{},
				_state.mapped_view,
				std::move(_state.staging_buffer),
				vulkan_image::make(image_cfg),
			};
		}

		if (!_state.image)
		{
			LOG_CRITICAL("failed to vulkan_image");
		}
		else
		{
			_state.res = res;
			_state.format = format;

			size_t src_offset = (char*)texture_data.data() - (char*)_state.mapped_view.data();
			buffer_mediator.copy(*_state.staging_buffer, *_state.image, (uint32_t)src_offset);
			if(generate_mipmaps)
			{
				buffer_mediator.generate_mipmaps(*_state.image);
			}
		}
	}

	if (_cfg.usage == buffer_usage::STATIC)
	{
		_state.staging_buffer.reset();
	}
}

const vulkan_staged_texture2d::config& vulkan_staged_texture2d::cfg() const
{
	return _cfg;
}

vulkan_staged_texture2d::source_type vulkan_staged_texture2d::source() const
{
	return _state.source;
}

const glm::ivec2& vulkan_staged_texture2d::res() const
{
	return _state.res;
}

texture_format vulkan_staged_texture2d::format() const
{
	return _state.format;
}

vulkan_image& vulkan_staged_texture2d::image()
{
	return *_state.image;
}