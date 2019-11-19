
#include "vulkan/defines/vulkan_includes.h"

#include <Windows.h>
#include <sstream>
#include <unordered_map>

using namespace igpu;
namespace
{
	struct
	{
		const char* file;
		int line;
		const char* func;
		const char* vk;
	} s_scope;

	std::pair< const char*, VkPipelineStageFlagBits > s_stage_pairs[] = {
		{ "VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT",					   //
		  VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT },					   //
		{ "VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT",				   //
		  VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT },				   //
		{ "VK_PIPELINE_STAGE_VERTEX_INPUT_BIT",					   //
		  VK_PIPELINE_STAGE_VERTEX_INPUT_BIT },					   //
		{ "VK_PIPELINE_STAGE_VERTEX_SHADER_BIT",				   //
		  VK_PIPELINE_STAGE_VERTEX_SHADER_BIT },				   //
		{ "VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT",	   //
		  VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT },	   //
		{ "VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT",  //
		  VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT },  //
		{ "VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT",				   //
		  VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT },				   //
		{ "VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT",				   //
		  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT },				   //
		{ "VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT",			   //
		  VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT },			   //
		{ "VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT",			   //
		  VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT },			   //
		{ "VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT",		   //
		  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT },		   //
		{ "VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT",				   //
		  VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT },				   //
		{ "VK_PIPELINE_STAGE_TRANSFER_BIT",						   //
		  VK_PIPELINE_STAGE_TRANSFER_BIT },						   //
		{ "VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT",				   //
		  VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT },				   //
		{ "VK_PIPELINE_STAGE_HOST_BIT",							   //
		  VK_PIPELINE_STAGE_HOST_BIT },							   //
		{ "VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT",					   //
		  VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT },					   //
		{ "VK_PIPELINE_STAGE_ALL_COMMANDS_BIT",					   //
		  VK_PIPELINE_STAGE_ALL_COMMANDS_BIT },					   //
		{ "VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT",		   //
		  VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT },		   //
		{ "VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT",	   //
		  VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT },	   //
		{ "VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX",			   //
		  VK_PIPELINE_STAGE_COMMAND_PROCESS_BIT_NVX },			   //
		{ "VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV",		   //
		  VK_PIPELINE_STAGE_SHADING_RATE_IMAGE_BIT_NV },		   //
		{ "VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV",		   //
		  VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_NV },		   //
		{ "VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV", //
		  VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_NV }, //
		{ "VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV",				   //
		  VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV },				   //
		{ "VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV",				   //
		  VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV },				   //
		{ "VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT",	   //
		  VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT },	   //
	};

	std::pair< const char*, VkAccessFlagBits > s_access_pairs[] = {
		{ "VK_ACCESS_INDIRECT_COMMAND_READ_BIT",				 //
		  VK_ACCESS_INDIRECT_COMMAND_READ_BIT },				 //
		{ "VK_ACCESS_INDEX_READ_BIT",							 //
		  VK_ACCESS_INDEX_READ_BIT },							 //
		{ "VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT",				 //
		  VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT },				 //
		{ "VK_ACCESS_UNIFORM_READ_BIT",							 //
		  VK_ACCESS_UNIFORM_READ_BIT },							 //
		{ "VK_ACCESS_INPUT_ATTACHMENT_READ_BIT",				 //
		  VK_ACCESS_INPUT_ATTACHMENT_READ_BIT },				 //
		{ "VK_ACCESS_SHADER_READ_BIT",							 //
		  VK_ACCESS_SHADER_READ_BIT },							 //
		{ "VK_ACCESS_SHADER_WRITE_BIT",							 //
		  VK_ACCESS_SHADER_WRITE_BIT },							 //
		{ "VK_ACCESS_COLOR_ATTACHMENT_READ_BIT",				 //
		  VK_ACCESS_COLOR_ATTACHMENT_READ_BIT },				 //
		{ "VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT",				 //
		  VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT },				 //
		{ "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT",		 //
		  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT },		 //
		{ "VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT",		 //
		  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT },		 //
		{ "VK_ACCESS_TRANSFER_READ_BIT",						 //
		  VK_ACCESS_TRANSFER_READ_BIT },						 //
		{ "VK_ACCESS_TRANSFER_WRITE_BIT",						 //
		  VK_ACCESS_TRANSFER_WRITE_BIT },						 //
		{ "VK_ACCESS_HOST_READ_BIT",							 //
		  VK_ACCESS_HOST_READ_BIT },							 //
		{ "VK_ACCESS_HOST_WRITE_BIT",							 //
		  VK_ACCESS_HOST_WRITE_BIT },							 //
		{ "VK_ACCESS_MEMORY_READ_BIT",							 //
		  VK_ACCESS_MEMORY_READ_BIT },							 //
		{ "VK_ACCESS_MEMORY_WRITE_BIT",							 //
		  VK_ACCESS_MEMORY_WRITE_BIT },							 //
		{ "VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT",			 //
		  VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT },			 //
		{ "VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT",	 //
		  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_READ_BIT_EXT },	 //
		{ "VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT",	 //
		  VK_ACCESS_TRANSFORM_FEEDBACK_COUNTER_WRITE_BIT_EXT },	 //
		{ "VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT",		 //
		  VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT },		 //
		{ "VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX",				 //
		  VK_ACCESS_COMMAND_PROCESS_READ_BIT_NVX },				 //
		{ "VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX",			 //
		  VK_ACCESS_COMMAND_PROCESS_WRITE_BIT_NVX },			 //
		{ "VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT", //
		  VK_ACCESS_COLOR_ATTACHMENT_READ_NONCOHERENT_BIT_EXT }, //
		{ "VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV",			 //
		  VK_ACCESS_SHADING_RATE_IMAGE_READ_BIT_NV },			 //
		{ "VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV",		 //
		  VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_NV },		 //
		{ "VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV",		 //
		  VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_NV },		 //
		{ "VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT",		 //
		  VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT },		 //
	};
}

static debug::callback_info s_callback_info = {
	logging::severity::VERBOSE,
};

#if ATMOS_DEBUG
void debug::set_callback_info( callback_info info )
{
	s_callback_info = info;
	logging::log_context(
		s_scope.file,
		s_scope.line,
		s_scope.func,
		s_callback_info.severity,
		"%s\n\t%s",
		s_scope.vk,
		s_callback_info.message );
}

bool debug::consume_debug_break()
{
	bool should_break = logging::severity::DEBUG > s_callback_info.severity;

	s_callback_info = {
		logging::severity::VERBOSE,
	};
	return should_break;
}

std::string debug::to_string( VkResult result )
{
	switch ( result )
	{
	case VK_SUCCESS:
		return "VK_SUCCESS";
	case VK_NOT_READY:
		return "VK_NOT_READY";
	case VK_TIMEOUT:
		return "VK_TIMEOUT";
	case VK_EVENT_SET:
		return "VK_EVENT_SET";
	case VK_EVENT_RESET:
		return "VK_EVENT_RESET";
	case VK_INCOMPLETE:
		return "VK_INCOMPLETE";
	case VK_ERROR_OUT_OF_HOST_MEMORY:
		return "VK_ERROR_OUT_OF_HOST_MEMORY";
	case VK_ERROR_OUT_OF_DEVICE_MEMORY:
		return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
	case VK_ERROR_INITIALIZATION_FAILED:
		return "VK_ERROR_INITIALIZATION_FAILED";
	case VK_ERROR_DEVICE_LOST:
		return "VK_ERROR_DEVICE_LOST";
	case VK_ERROR_MEMORY_MAP_FAILED:
		return "VK_ERROR_MEMORY_MAP_FAILED";
	case VK_ERROR_LAYER_NOT_PRESENT:
		return "VK_ERROR_LAYER_NOT_PRESENT";
	case VK_ERROR_EXTENSION_NOT_PRESENT:
		return "VK_ERROR_EXTENSION_NOT_PRESENT";
	case VK_ERROR_FEATURE_NOT_PRESENT:
		return "VK_ERROR_FEATURE_NOT_PRESENT";
	case VK_ERROR_INCOMPATIBLE_DRIVER:
		return "VK_ERROR_INCOMPATIBLE_DRIVER";
	case VK_ERROR_TOO_MANY_OBJECTS:
		return "VK_ERROR_TOO_MANY_OBJECTS";
	case VK_ERROR_FORMAT_NOT_SUPPORTED:
		return "VK_ERROR_FORMAT_NOT_SUPPORTED";
	case VK_ERROR_FRAGMENTED_POOL:
		return "VK_ERROR_FRAGMENTED_POOL";
	case VK_ERROR_OUT_OF_POOL_MEMORY:
		return "VK_ERROR_OUT_OF_POOL_MEMORY";
	case VK_ERROR_INVALID_EXTERNAL_HANDLE:
		return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
	case VK_ERROR_SURFACE_LOST_KHR:
		return "VK_ERROR_SURFACE_LOST_KHR";
	case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
		return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
	case VK_SUBOPTIMAL_KHR:
		return "VK_SUBOPTIMAL_KHR";
	case VK_ERROR_OUT_OF_DATE_KHR:
		return "VK_ERROR_OUT_OF_DATE_KHR";
	case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
		return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
	case VK_ERROR_VALIDATION_FAILED_EXT:
		return "VK_ERROR_VALIDATION_FAILED_EXT";
	case VK_ERROR_INVALID_SHADER_NV:
		return "VK_ERROR_INVALID_SHADER_NV";
	case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT:
		return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
	case VK_ERROR_FRAGMENTATION_EXT:
		return "VK_ERROR_FRAGMENTATION_EXT";
	case VK_ERROR_NOT_PERMITTED_EXT:
		return "VK_ERROR_NOT_PERMITTED_EXT";
	case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:
		return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
	case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
		return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
	case VK_RESULT_MAX_ENUM:
		return "VK_RESULT_MAX_ENUM";
	}

	return "<invalid result>";
}

std::string debug::to_string( VkImageLayout layout )
{
	switch ( layout )
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		return "VK_IMAGE_LAYOUT_UNDEFINED";
	case VK_IMAGE_LAYOUT_GENERAL:
		return "VK_IMAGE_LAYOUT_GENERAL";
	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		return "VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL";
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL";
	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
		return "VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL";
	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		return "VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL";
	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		return "VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL";
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		return "VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL";
	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		return "VK_IMAGE_LAYOUT_PREINITIALIZED";
	case VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL:
		return "VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL";
	case VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL:
		return "VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL";
	case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
		return "VK_IMAGE_LAYOUT_PRESENT_SRC_KHR";
	case VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR:
		return "VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR";
	case VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV:
		return "VK_IMAGE_LAYOUT_SHADING_RATE_OPTIMAL_NV";
	case VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT:
		return "VK_IMAGE_LAYOUT_FRAGMENT_DENSITY_MAP_OPTIMAL_EXT";
	}

	return "<invalid layout>";
}

std::string debug::to_stages_string( VkPipelineStageFlags stages )
{
	std::string result;

	if ( stages == VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM )
	{
		return "VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM";
	}
	for ( const auto& pair : s_stage_pairs )
	{
		if ( stages & pair.second )
		{
			if ( !result.empty() )
			{
				result += ", ";
			}

			result += pair.first;
		}
	}

	return result;
}

std::string debug::to_access_string( VkAccessFlags access )
{
	std::string result;

	if ( access == VK_ACCESS_FLAG_BITS_MAX_ENUM )
	{
		return "VK_ACCESS_FLAG_BITS_MAX_ENUM";
	}

	for ( const auto& pair : s_access_pairs )
	{
		if ( access & pair.second )
		{
			if ( !result.empty() )
			{
				result += ", ";
			}

			result += pair.first;
		}
	}

	return result;
}

void debug::validate(
	const char* file,
	int line,
	const char* func,
	const char* vk,
	const std::function< void() >& fun )
{
	s_scope = {
		file,
		line,
		func,
		vk,
	};

	fun();
}

VkResult debug::validate(
	const char* file,
	int line,
	const char* func,
	const char* vk,
	const std::function< VkResult() >& fun )
{
	s_scope = {
		file,
		line,
		func,
		vk,
	};

	auto res = fun();
	if ( res != VK_SUCCESS && res != VK_NOT_READY && res != VK_TIMEOUT &&
		 res != VK_EVENT_SET && res != VK_EVENT_RESET && res != VK_INCOMPLETE &&
		 res != VK_SUBOPTIMAL_KHR && res != VK_ERROR_OUT_OF_DATE_KHR &&
		 res != VK_RESULT_BEGIN_RANGE && res != VK_RESULT_END_RANGE &&
		 res != VK_RESULT_RANGE_SIZE )
	{
		std::ostringstream oss;
		oss << " == " << to_string( res ) << std::endl;
		auto str = oss.str();

		set_callback_info( {
			logging::severity::CRITICAL,
			str.c_str(),
		} );
	}
	return res;
}

#endif