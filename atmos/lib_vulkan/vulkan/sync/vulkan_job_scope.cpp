
#include "vulkan/sync/vulkan_job_scope.h"

using namespace igpu;

namespace
{
	bool check_access(
		decorator deco,
		decorator check_deco,
		VkAccessFlags access,
		VkAccessFlags check_access )
	{
		if ( !( deco & check_deco ) != !( access & check_access ) )
		{
			LOG_CRITICAL(
				"%s is not compatible with \n%s \n%s \nis compatible with \n%s",
				debug::to_access_string( access ).c_str(),
				to_string( deco ).data(),
				debug::to_access_string( check_access ).c_str(),
				to_string( check_deco ).data() );
			return false;
		}

		return true;
	}

	bool check_access(
		VkAccessFlags access,
		VkAccessFlags check_access,
		VkPipelineStageFlags stages,
		VkPipelineStageFlags check_stages )
	{
		if ( !( stages & check_stages ) != !( access & check_access ) )
		{
			LOG_CRITICAL(
				"%s \nis not compatible with \n%s \n%s \nis compatible with "
				"\n%s",
				debug::to_access_string( access ).c_str(),
				debug::to_stages_string( stages ).c_str(),
				debug::to_access_string( check_access ).c_str(),
				debug::to_stages_string( check_stages ).data() );
			return false;
		}

		return true;
	}

	bool check_access(
		decorator decorators,
		VkPipelineStageFlags stages,
		VkAccessFlags access )
	{
		bool valid = true;

		valid &= check_access(
			decorators,
			decorator::READABLE,
			access,
			( VK_ACCESS_INDIRECT_COMMAND_READ_BIT | VK_ACCESS_INDEX_READ_BIT |
			  VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT |
			  VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
			  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			  VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_HOST_READ_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT |
			  VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX |
			  VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT |
			  VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV |
			  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT |
			  VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV |
			  VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT ) );

		valid &= check_access(
			decorators,
			decorator::WRITABLE,
			access,
			( VK_ACCESS_SHADER_WRITE_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_TRANSFER_WRITE_BIT | VK_ACCESS_HOST_WRITE_BIT |
			  VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX |
			  VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT |
			  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT |
			  VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV ) );

		valid &= check_access(
			access,
			( VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT |
			  VK_ACCESS_INDIRECT_COMMAND_READ_BIT ),
			stages,
			( VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT ),
			stages,
			( VK_PIPELINE_STAGE_VERTEX_INPUT_BIT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT |
			  VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
			  VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV |
			  VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV ),
			stages,
			( VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
			  VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
			  VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
			  VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
			  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
			  VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT |
			  VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV |
			  VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV |
			  VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV |
			  VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV ) );

		valid &= check_access(
			access,
			( VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			  VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT ),
			stages,
			( VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
			  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT ),
			stages,
			( VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
			  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT ),
			stages,
			( VK_PIPELINE_STAGE_TRANSFER_BIT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT ),
			stages,
			( VK_PIPELINE_STAGE_HOST_BIT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX |
			  VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX ),
			stages,
			( VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX ) );

		valid &= check_access(
			access,
			( VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT ),
			stages,
			( VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV ),
			stages,
			( VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV ) );

		valid &= check_access(
			access,
			( VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT |
			  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT ),
			stages,
			( VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT ) );

		valid &= check_access(
			access,
			( VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT ),
			stages,
			( VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT ) );

		return valid;
	}
}

bool vulkan_job_scope::validate() const
{
	bool valid = true;

	if ( !decorators )
	{
		LOG_CRITICAL( "decorators is zero" );
		valid = false;
	}

	if ( !stages )
	{
		LOG_CRITICAL( "stages is zero" );
		valid = false;
	}

	if ( !access )
	{
		LOG_CRITICAL( "access is zero" );
		valid = false;
	}

	if ( !check_access( decorators, stages, access ) )
	{
		valid = false;
	}

	return valid;
}

bool vulkan_job_scope::is_empty() const
{
	return 0 == decorators && 0 == access &&
		( 0 == stages || VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT == stages );
}

bool vulkan_job_scope::is_readable() const
{
	return 0 != ( decorators & decorator::READABLE );
}

bool vulkan_job_scope::is_writable() const
{
	return 0 != ( decorators & decorator::WRITABLE );
}

bool vulkan_job_scope::contains( const vulkan_job_scope& other ) const
{
	return decorators == ( decorators | other.decorators ) &&
		stages == ( stages | other.stages ) &&
		access == ( access | other.access );
}

vulkan_job_scope vulkan_job_scope::combined(
	const vulkan_job_scope& other ) const
{
	return {
		decorators | other.decorators,
		( VkPipelineStageFlagBits )(
			(VkPipelineStageFlags)stages | (VkPipelineStageFlags)other.stages ),
		( VkAccessFlagBits )(
			(VkAccessFlags)access | (VkAccessFlags)other.access ),
	};
}