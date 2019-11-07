
#pragma once

#include "vulkan/shader/vulkan_shader_stages.h"

using namespace igpu;

VkShaderStageFlagBits igpu::to_vulkan_shader_stage_flags( shader_stages stages )
{
	static_assert(
		(size_t)VK_SHADER_STAGE_VERTEX_BIT == (size_t)shader_stages::VERTEX );
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
		(size_t)VK_SHADER_STAGE_COMPUTE_BIT == (size_t)shader_stages::COMPUTE );
	static_assert( shader_stages::ALL == ( shader_stages::COMPUTE << 1 ) - 1 );

	return (VkShaderStageFlagBits)stages;
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

VkPipelineStageFlags igpu::to_vulkan_pipeline_stage_flags(
	shader_stages stages )
{
	auto error_stages = stages & ~shader_stages::ALL;
	if ( error_stages )
	{
		LOG_CRITICAL( "unhandled stage flags %x", error_stages );
	}
	VkPipelineStageFlags vulkan = {};

	if ( stages & shader_stages::VERTEX )
	{
		vulkan |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
	}
	if ( stages & shader_stages::TESSELLATION_CONTROL )
	{
		vulkan |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
	}
	if ( stages & shader_stages::TESSELLATION_EVALUATION )
	{
		vulkan |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
	}
	if ( stages & shader_stages::GEOMETRY )
	{
		vulkan |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
	}
	if ( stages & shader_stages::FRAGMENT )
	{
		vulkan |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	if ( stages & shader_stages::COMPUTE )
	{
		vulkan |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
	}

	return vulkan;
}
