
#include <vulkan/defines/vulkan_includes.h>
#include <unordered_map>
#include <sstream>
#include <Windows.h>

#if ATMOS_DEBUG

#define VK_RESULT_XMACRO(OP)\
OP(VK_SUCCESS)\
OP(VK_NOT_READY)\
OP(VK_TIMEOUT)\
OP(VK_EVENT_SET)\
OP(VK_EVENT_RESET)\
OP(VK_INCOMPLETE)\
OP(VK_ERROR_OUT_OF_HOST_MEMORY)\
OP(VK_ERROR_OUT_OF_DEVICE_MEMORY)\
OP(VK_ERROR_INITIALIZATION_FAILED)\
OP(VK_ERROR_DEVICE_LOST)\
OP(VK_ERROR_MEMORY_MAP_FAILED)\
OP(VK_ERROR_LAYER_NOT_PRESENT)\
OP(VK_ERROR_EXTENSION_NOT_PRESENT)\
OP(VK_ERROR_FEATURE_NOT_PRESENT)\
OP(VK_ERROR_INCOMPATIBLE_DRIVER)\
OP(VK_ERROR_TOO_MANY_OBJECTS)\
OP(VK_ERROR_FORMAT_NOT_SUPPORTED)\
OP(VK_ERROR_FRAGMENTED_POOL)\
OP(VK_ERROR_OUT_OF_POOL_MEMORY)\
OP(VK_ERROR_INVALID_EXTERNAL_HANDLE)\
OP(VK_ERROR_SURFACE_LOST_KHR)\
OP(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)\
OP(VK_SUBOPTIMAL_KHR)\
OP(VK_ERROR_OUT_OF_DATE_KHR)\
OP(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)\
OP(VK_ERROR_VALIDATION_FAILED_EXT)\
OP(VK_ERROR_INVALID_SHADER_NV)\
OP(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)\
OP(VK_ERROR_FRAGMENTATION_EXT)\
OP(VK_ERROR_NOT_PERMITTED_EXT)\
OP(VK_ERROR_INVALID_DEVICE_ADDRESS_EXT)\
OP(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)\
OP(VK_ERROR_OUT_OF_POOL_MEMORY_KHR)\
OP(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR)\
OP(VK_RESULT_BEGIN_RANGE)\
OP(VK_RESULT_END_RANGE)\
OP(VK_RESULT_RANGE_SIZE)\
OP(VK_RESULT_MAX_ENUM)\
// end of xmacro

std::unordered_map<VkResult, const char*> kResultStr = {
#define VK_RESULT_PAIR(VK_RESULT) { VK_RESULT, #VK_RESULT },
VK_RESULT_XMACRO(VK_RESULT_PAIR)
#undef VK_RESULT_PAIR
};

using namespace igpu;

struct
{
	const char* file;
	int line;
	const char* func;
	const char* vk;
} s_scope;

static debug::callback_info s_callback_info = { logging::severity::VERBOSE };

void debug::set_callback_info(callback_info info)
{
	s_callback_info = info;
	logging::log_context(
		s_scope.file,
		s_scope.line,
		s_scope.func,
		s_callback_info.severity,
		"%s\n\t%s",
		s_scope.vk,
		s_callback_info.message);
}

bool debug::consume_debug_break()
{
	bool should_break =
		logging::severity::DEBUG > s_callback_info.severity;
	
	s_callback_info = { logging::severity::VERBOSE  };
	return should_break;
}


std::string debug::stringify_result(VkResult res)
{
	vmaCreateAllocator(nullptr, nullptr);

	return  kResultStr.at(res);
}

void debug::validate(const char* file, int line, const char* func, const char* vk, const std::function<void()>& fun)
{
	s_scope = {
		file,
		line,
		func,
		vk
	};

	fun();
}

VkResult debug::validate(const char* file, int line, const char* func, const char* vk, const std::function<VkResult()>& fun)
{
	s_scope = {
		file,
		line,
		func,
		vk
	};
	
	auto res = fun();
	if (res != VK_SUCCESS &&
		res != VK_NOT_READY &&
		res != VK_TIMEOUT &&
		res != VK_EVENT_SET &&
		res != VK_EVENT_RESET &&
		res != VK_INCOMPLETE &&
		res != VK_SUBOPTIMAL_KHR &&
		res != VK_ERROR_OUT_OF_DATE_KHR &&
		res != VK_RESULT_BEGIN_RANGE &&
		res != VK_RESULT_END_RANGE &&
		res != VK_RESULT_RANGE_SIZE)
	{
		std::ostringstream oss;
		oss
			<< " == "  << kResultStr.at(res) << std::endl;
		auto str = oss.str();
		
		set_callback_info({
			logging::severity::CRITICAL,
			str.c_str()
			});
	}
	return res;
}

#endif // ATMOS_DEBUG
