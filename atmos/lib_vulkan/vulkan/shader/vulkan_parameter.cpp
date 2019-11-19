
#include "vulkan/shader/vulkan_parameter.h"

using namespace igpu;

namespace
{
	VkDescriptorType to_vulkan_type( parameter::type type )
	{
		switch ( type )
		{
		case parameter::type::UNIFORM_BUFFER:
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

		case parameter::type::STORAGE_BUFFER:
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;

		case parameter::type::SAMPLER2D:
			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		};

		if ( parameter::is_valid( type ) )
		{
			LOG_CRITICAL(
				"unhandled type: %s",
				parameter::to_string( type ).data() );
		}
		else
		{
			LOG_CRITICAL( "invalid type: %d", type );
		}

		return (VkDescriptorType)-1;
	}

	VkImageLayout to_vulkan_image_layout( parameter::type type )
	{
		switch ( type )
		{
		case parameter::type::UNIFORM_BUFFER:
			return VK_IMAGE_LAYOUT_MAX_ENUM;

		case parameter::type::STORAGE_BUFFER:
			return VK_IMAGE_LAYOUT_MAX_ENUM;

		case parameter::type::SAMPLER2D:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		};

		if ( parameter::is_valid( type ) )
		{
			LOG_CRITICAL(
				"unhandled type: %s",
				parameter::to_string( type ).data() );
		}
		else
		{
			LOG_CRITICAL( "invalid type: %d", type );
		}

		return VK_IMAGE_LAYOUT_MAX_ENUM;
	}

	VkShaderStageFlagBits to_vulkan_shader_stage_flags( shader_stages stages )
	{
		static_assert(
			(size_t)VK_SHADER_STAGE_VERTEX_BIT ==
			(size_t)shader_stages::VERTEX );
		static_assert(
			(size_t)VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT ==
			(size_t)shader_stages::TESSELLATION_CONTROL );
		static_assert(
			(size_t)VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT ==
			(size_t)shader_stages::TESSELLATION_EVALUATION );
		static_assert(
			(size_t)VK_SHADER_STAGE_GEOMETRY_BIT ==
			(size_t)shader_stages::GEOMETRY );
		static_assert(
			(size_t)VK_SHADER_STAGE_FRAGMENT_BIT ==
			(size_t)shader_stages::FRAGMENT );
		static_assert(
			(size_t)VK_SHADER_STAGE_COMPUTE_BIT ==
			(size_t)shader_stages::COMPUTE );
		static_assert(
			shader_stages::ALL ==
			shader_stages( int( shader_stages::COMPUTE << 1 ) - 1 ) );

		return (VkShaderStageFlagBits)stages;
	}

	VkPipelineStageFlagBits to_vulkan_pipeline_stage_flags(
		shader_stages stages )
	{
		auto error_stages = stages & ~shader_stages::ALL;
		if ( 0 != error_stages )
		{
			LOG_CRITICAL( "unhandled stage flags %x", error_stages );
		}

		VkPipelineStageFlags vulkan = {};

		if ( 0 != ( stages & shader_stages::VERTEX ) )
		{
			vulkan |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		}
		if ( 0 != ( stages & shader_stages::TESSELLATION_CONTROL ) )
		{
			vulkan |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		}
		if ( 0 != ( stages & shader_stages::TESSELLATION_EVALUATION ) )
		{
			vulkan |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		}
		if ( 0 != ( stages & shader_stages::GEOMETRY ) )
		{
			vulkan |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		}
		if ( 0 != ( stages & shader_stages::FRAGMENT ) )
		{
			vulkan |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		if ( 0 != ( stages & shader_stages::COMPUTE ) )
		{
			vulkan |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}

		return (VkPipelineStageFlagBits)vulkan;
	}
}

shader_stages igpu::from_vulkan_shader_stage_flags(
	VkShaderStageFlagBits flags )
{
	if ( shader_stages::ALL < (shader_stages)flags )
	{
		LOG_CRITICAL( "unexpected VkShaderStageFlagBits %d", (int)flags );
	}

	return shader_stages::ALL & (shader_stages)flags;
}

vulkan_parameter::config::vulkan igpu::to_vulkan( const parameter::config& cfg )
{
	return {
		to_vulkan_type( cfg.type ),
		to_vulkan_image_layout( cfg.type ),
		to_vulkan_shader_stage_flags( cfg.shader_stages ),
		to_vulkan_pipeline_stage_flags( cfg.shader_stages ),
	};
}

const vulkan_parameter::config& vulkan_parameter::cfg() const
{
	return _cfg;
}

vulkan_parameter::vulkan_parameter( const parameter::config& cfg )
	: _cfg{ cfg, to_vulkan( cfg ) }
{}
