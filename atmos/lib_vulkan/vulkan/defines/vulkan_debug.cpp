
#include "vulkan/defines/vulkan_includes.h"

#include <Windows.h>
#include <sstream>
#include <unordered_map>

using namespace igpu;

struct
{
	const char* file;
	int line;
	const char* func;
	const char* vk;
} s_scope;

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
	case VK_IMAGE_LAYOUT_MAX_ENUM:
		return "VK_IMAGE_LAYOUT_MAX_ENUM";
	}

	return "<invalid layout>";
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