
#include "vulkan/texture/vulkan_staged_texture2d.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/sync/vulkan_synchronization.h"
#include "vulkan/texture/vulkan_image.h"
#include "vulkan/texture/vulkan_image_t.h"

#include "igpu/texture/texture_file_parsing.h"

#include "framework/logging/log.h"

using namespace igpu;

vulkan_staged_texture2d::vulkan_staged_texture2d(
	const config& cfg,
	const scoped_ptr< vulkan_synchronization >& synchronization )
	: _cfg( cfg )
	, _synchronization( synchronization )
	, _staging_buffer( {
		  cfg.memory,
		  cfg.vk.device_properties,
		  synchronization->vma(),
		  VMA_MEMORY_USAGE_CPU_ONLY,
		  VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	  } )
{}

vulkan_staged_texture2d::~vulkan_staged_texture2d()
{}

const vulkan_staged_texture2d::config& vulkan_staged_texture2d::cfg() const
{
	return _cfg;
}

void vulkan_staged_texture2d::map( buffer_view_base* out_buffer_view )
{
	_staging_buffer.map( out_buffer_view );
	_mapped_view = buffer_view< char >(
		out_buffer_view->byte_size(),
		(char*)out_buffer_view->data() );
}

void vulkan_staged_texture2d::unmap( const texture2d::state& state )
{
	if ( !_staging_buffer.mapped_view().data() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		_staging_buffer.unmap();
		upload( _mapped_view, state );
	}
}

void vulkan_staged_texture2d::unmap()
{
	if ( !_staging_buffer.mapped_view().data() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		buffer_view< char > parsed_buffer_view = {};
		texture2d::state state = {};

		if ( !texture_file_parsing::parse_as_ktx(
				 _mapped_view,
				 &parsed_buffer_view,
				 &state ) )
		{
			if ( !texture_file_parsing::parse_as_dds(
					 _mapped_view,
					 &parsed_buffer_view,
					 &state ) )
			{
				if ( !texture_file_parsing::parse_as_pvr(
						 _mapped_view,
						 &parsed_buffer_view,
						 &state ) )
				{
					texture_file_parsing::compressed_parser compressed_parser =
						_mapped_view;

					if ( compressed_parser.format != texture_format::UNDEFINED )
					{
						// if there is decompressed data to use, replace the
						// contents of the staging buffer with it
						state.res = compressed_parser.res;
						state.format = compressed_parser.format;

						_staging_buffer.unmap();

						parsed_buffer_view = buffer_view< char >(
							compressed_parser.decompressed.size(),
							nullptr );

						map( &parsed_buffer_view );

						memcpy(
							parsed_buffer_view.data(),
							compressed_parser.decompressed.data(),
							compressed_parser.decompressed.size() );

						_staging_buffer.unmap();

						upload( parsed_buffer_view, state );
					}

					return;
				}
			}
		}

		_staging_buffer.unmap();
		upload( parsed_buffer_view, state );
	}
}

const vulkan_staged_texture2d::state& vulkan_staged_texture2d::current_state()
	const
{
	return _current_state;
}

vulkan_image& vulkan_staged_texture2d::gpu_object()
{
	ASSERT_CONTEXT( (bool)_gpu_image );
	return *_gpu_image;
}

const vulkan_image& vulkan_staged_texture2d::gpu_object() const
{
	ASSERT_CONTEXT( (bool)_gpu_image );
	return *_gpu_image;
}

void vulkan_staged_texture2d::upload(
	const buffer_view< char >& texture_data,
	const texture2d::state& state )
{
	if ( !_mapped_view.data() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
		return;
	}

	VkFormat vulkan_format = to_vulkan_format( state.format );

	size_t src_offset = texture_data.data() - _mapped_view.data();
	ASSERT_CONTEXT( texture_data.data() );
	ASSERT_CONTEXT( _mapped_view.data() );
	ASSERT_CONTEXT( src_offset < _mapped_view.byte_size() );

	_mapped_view = buffer_view< char >( _mapped_view.byte_size(), nullptr );

	if ( vulkan_format )
	{
		auto& synchronization = *_synchronization;

		bool generate_mipmaps = false;
		size_t mipmap_count = state.mipmap_count;
		if ( _cfg.can_auto_generate_mips && state.mipmap_count == 1 )
		{
			if ( synchronization.can_generate_mipmaps(
					 vulkan_format,
					 VK_IMAGE_TILING_OPTIMAL ) )
			{
				generate_mipmaps = true;
				auto max_dim =
					state.res.x > state.res.y ? state.res.x : state.res.y;
				mipmap_count =
					static_cast< uint32_t >( std::log2( max_dim ) ) + 1;
			}
		}

		if ( !_gpu_image || _current_state.res != state.res ||
			 _current_state.format != state.format ||
			 _current_state.mipmap_count != mipmap_count )
		{
			vulkan_image::config image_cfg = to_vulkan_image_info(
				_cfg,
				state.res,
				vulkan_format,
				VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
					VK_IMAGE_USAGE_TRANSFER_DST_BIT |
					VK_IMAGE_USAGE_SAMPLED_BIT,
				VK_SAMPLE_COUNT_1_BIT,
				VK_SHARING_MODE_EXCLUSIVE,
				VK_IMAGE_ASPECT_COLOR_BIT,
				mipmap_count );

			_gpu_image = nullptr;

			if ( vulkan_image::validate( image_cfg ) )
			{
				_gpu_image = vulkan_image::make( image_cfg );
			}

			if ( !_gpu_image )
			{
				LOG_CRITICAL( "failed to create vulkan_image" );
				return;
			}
		}

		_current_state = state;
		_current_state.mipmap_count = mipmap_count;

		synchronization.copy(
			_staging_buffer,
			*_gpu_image,
			(uint32_t)src_offset );

		if ( generate_mipmaps )
		{
			synchronization.generate_mipmaps( *_gpu_image );
		}
	}

	if ( _cfg.memory == memory_type::WRITE_COMBINED )
	{
		_staging_buffer.release();
	}
}