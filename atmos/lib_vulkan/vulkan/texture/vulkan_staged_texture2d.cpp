
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
	, _staging_buffer({
		cfg.usage,
		buffer_mediator->vma(),
		VMA_MEMORY_USAGE_CPU_ONLY,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		})
{
}

vulkan_staged_texture2d::~vulkan_staged_texture2d()
{
}

const vulkan_staged_texture2d::config& vulkan_staged_texture2d::cfg() const
{
	return _cfg;
}

void vulkan_staged_texture2d::map(size_t byte_size, buffer_view_base* out_buffer_view)
{
	_staging_buffer.map(byte_size, out_buffer_view);
	_mapped_view = *out_buffer_view;
}

void vulkan_staged_texture2d::unmap(
	const state& state)
{
	if (!_staging_buffer.mapped_view().data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		unmap(
			_mapped_view,
			state);
	}
}

void vulkan_staged_texture2d::unmap()
{
	if (!_staging_buffer.mapped_view().data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		buffer_view<char> parsed_buffer_view = {};
		texture2d::state state = {};

		if (!texture_file_parsing::parse_as_ktx(_mapped_view, &parsed_buffer_view, &state))
		{
			if (!texture_file_parsing::parse_as_dds(_mapped_view, &parsed_buffer_view, &state))
			{
				if (!texture_file_parsing::parse_as_pvr(_mapped_view, &parsed_buffer_view, &state))
				{
					texture_file_parsing::compressed_parser compressed_parser = _mapped_view;

					if (compressed_parser.format != texture_format::UNDEFINED)
					{
						state.res = compressed_parser.res;
						state.format = compressed_parser.format;
						
						_staging_buffer.unmap();
						map(compressed_parser.decompressed.size(), &parsed_buffer_view);
						
						memcpy(
							parsed_buffer_view.data(),
							compressed_parser.decompressed.data(),
							compressed_parser.decompressed.size());


						unmap(
							parsed_buffer_view,
							state);
					}

					return;
				}
			}
		}

		unmap(parsed_buffer_view, state);
	}
}

size_t vulkan_staged_texture2d::byte_capacity() const
{
	return _staging_buffer.byte_capacity();
}

const vulkan_staged_texture2d::state& vulkan_staged_texture2d::current_state() const
{
	return _state;
}

void vulkan_staged_texture2d::unmap(
	const buffer_view<char>& texture_data,
	const state& state)
{
	if (!_mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
		return;
	}

	VkFormat vulkan_format = to_vulkan_format(state.format);

	size_t src_offset = (char*)texture_data.data() - (char*)_mapped_view.data();
	ASSERT_CONTEXT(texture_data.data());
	ASSERT_CONTEXT(_mapped_view.data());
	ASSERT_CONTEXT(src_offset < _staging_buffer.byte_capacity());

	_staging_buffer.unmap();
	_mapped_view =
		buffer_view<char>(
			_mapped_view.byte_size(),
			nullptr);

	if (vulkan_format)
	{
		auto& buffer_mediator = *_buffer_mediator;
		
		bool generate_mipmaps = false;
		size_t mipmap_count = state.mipmap_count;
		if (_cfg.can_auto_generate_mips && state.mipmap_count == 1)
		{
			if(buffer_mediator.can_generate_mipmaps(
				vulkan_format,
				VK_IMAGE_TILING_OPTIMAL))
			{
				generate_mipmaps = true;
				auto max_dim = state.res.x > state.res.y ? state.res.x : state.res.y;
				mipmap_count = static_cast<uint32_t>(std::log2(max_dim)) + 1;
			}
		}

		if (!_image ||
			_state.res != state.res ||
			_state.format != state.format ||
			_state.mipmap_count != mipmap_count)
		{
			VkImageCreateInfo image_info = {};
			image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			image_info.imageType = VK_IMAGE_TYPE_2D;
			image_info.extent = { (uint32_t)state.res.x, (uint32_t)state.res.y , 1 };
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
			image_cfg.vk.physical_device = buffer_mediator.cfg().physical_device;
			image_cfg.vk.device = buffer_mediator.cfg().device;
			image_cfg.vk.memory_properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			image_cfg.vk.image_info = image_info;
			image_cfg.vk.view_info = view_info;
			image_cfg.vk.sampler_info = sampler_info;


			_image = nullptr;
			_image = vulkan_image::make(image_cfg);

			if (!_image)
			{
				LOG_CRITICAL("failed to vulkan_image");
				return;
			}
		}
		
		_state = state;
		_state.mipmap_count = mipmap_count;

		buffer_mediator.copy(_staging_buffer, *_image, (uint32_t)src_offset);
		
		if(generate_mipmaps)
		{
			buffer_mediator.generate_mipmaps(*_image);
		}
	}

	if (_cfg.usage == buffer_usage::STATIC)
	{
		_staging_buffer.release();
	}
}

vulkan_image& vulkan_staged_texture2d::image()
{
	return *_image;
}