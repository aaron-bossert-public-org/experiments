#pragma once

#define ATMOS_USE_VULKAN 1

#if ATMOS_BUILD_IOS
#	define VK_USE_PLATFORM_IOS_MVK 1
#elif ATMOS_BUILD_ANDROID
#	define VK_USE_PLATFORM_ANDROID_KHR 1
#elif ATMOS_BUILD_WINDOWS
#	define VK_USE_PLATFORM_WIN32_KHR 1
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#elif ATMOS_BUILD_OSX
#	define VK_USE_PLATFORM_MACOS_MVK 1
#else
#	error No build platform defined ATMOS_BUILD_IOS/ ATMOS_BUILD_WINDOWS/ etc...
#endif

#include "framework/logging/log.h"

#include <functional>
#include <vulkan.h>

#pragma warning( push )
// local variable not initialized, vk_mem_alloc.h
#pragma warning( disable : 4189 )

#define VMA_ASSERT( ... )		ASSERT_CONTEXT( __VA_ARGS__ )
#define VMA_HEAVY_ASSERT( ... ) ASSERT_CONTEXT( __VA_ARGS__ )
#define VMA_DEBUG_LOG( ... ) \
	LOG_VERBOSE( "VulkanMemoryAllocator: " __VA_ARGS__ )

#include <VulkanMemoryAllocator\src\vk_mem_alloc.h>
#pragma warning( pop )

#if ATMOS_DEBUG
#	include <vulkan/defines/vulkan_debug.h>
#endif

#if ATMOS_BUILD_WINDOWS
#	undef far
#	undef near
#endif

namespace igpu
{
	struct debug
	{
		struct callback_info
		{
			logging::severity severity;
			const char* message;
		};

		static std::string stringify_result( VkResult );
#if ATMOS_DEBUG
		static void validate(
			const char* file,
			int line,
			const char* func,
			const char* vk,
			const std::function< void() >& fun );

		static VkResult validate(
			const char* file,
			int line,
			const char* func,
			const char* vk,
			const std::function< VkResult() >& fun );

		static void set_callback_info( callback_info );

		static bool consume_debug_break();
#endif
	};
}
