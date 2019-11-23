
#include "vulkan/texture/vulkan_texture2d.h"

#include "vulkan/texture/vulkan_staged_texture2d.h"

using namespace igpu;

VkFormat igpu::to_vulkan_format( texture_format format )
{
	switch ( format )
	{
	case texture_format::INT_R8:
		return VK_FORMAT_R8_UNORM;

	case texture_format::INT_R8G8:
		return VK_FORMAT_R8G8_UNORM;

	case texture_format::INT_R8G8B8:
		return VK_FORMAT_R8G8B8_UNORM;

	case texture_format::INT_R8G8B8A8:
		return VK_FORMAT_R8G8B8A8_UNORM;


	case texture_format::INT_R16:
		return VK_FORMAT_R16_UNORM;

	case texture_format::INT_R16G16:
		return VK_FORMAT_R16G16_UNORM;

	case texture_format::INT_R16G16B16:
		return VK_FORMAT_R16G16B16_UNORM;

	case texture_format::INT_R16G16B16A16:
		return VK_FORMAT_R16G16B16A16_UNORM;


	case texture_format::PVR1_2BPP_RGB:
		return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;

	case texture_format::PVR1_2BPP_RGBA:
		return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;

	case texture_format::PVR2_2BPP:
		return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;


	case texture_format::PVR1_4BPP_RGB:
		return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;

	case texture_format::PVR1_4BPP_RGBA:
		return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;

	case texture_format::PVR2_4BPP:
		return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;

	case texture_format::ETC1_RGB:
		return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;


	case texture_format::DXT_1_RGBA:
		return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;

	case texture_format::DXT_3_RGBA:
		return VK_FORMAT_BC2_UNORM_BLOCK;

	case texture_format::DXT_5_RGBA:
		return VK_FORMAT_BC3_UNORM_BLOCK;
	};

	if ( is_valid( format ) )
	{
		LOG_CRITICAL( "unhandled format: %s", to_string( format ).data() );
	}
	else
	{
		LOG_CRITICAL( "invalid format: %d", format );
	}

	return VK_FORMAT_UNDEFINED;
}

std::unique_ptr< vulkan_texture2d > vulkan_texture2d::make( const config& cfg )
{
	return std::unique_ptr< vulkan_texture2d >(
		new vulkan_staged_texture2d( cfg ) );
}