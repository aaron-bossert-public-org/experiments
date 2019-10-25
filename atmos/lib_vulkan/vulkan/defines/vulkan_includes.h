#pragma once

#define ATMOS_USE_VULKAN 1

#include <framework/logging/log.h>

#if ATMOS_BUILD_IOS
#	define VK_USE_PLATFORM_IOS_MVK 1
#elif ATMOS_BUILD_ANDROID
#	define VK_USE_PLATFORM_ANDROID_KHR 1
#elif ATMOS_BUILD_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR 1
#elif ATMOS_BUILD_OSX
	#define VK_USE_PLATFORM_MACOS_MVK 1
#else
#	error No build platform defined ATMOS_BUILD_IOS/ ATMOS_BUILD_WINDOWS/ etc...
#endif


#if ATMOS_BUILD_WINDOWS
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#endif

#pragma warning(push)
#pragma warning(disable:4189)// local variable not initialized, vk_mem_alloc.h
#if ATMOS_DEBUG
#	include <vulkan/defines/vulkan_debug.h>
#else
#	define VULKAN_CHECK_ERR(f, ...) f(__VA_ARGS__)
#	include <vulkan.h>
#	include <VulkanMemoryAllocator\src\vk_mem_alloc.h>
#endif
#pragma warning(pop)

#if ATMOS_BUILD_WINDOWS
#	undef far
#	undef near
#endif
