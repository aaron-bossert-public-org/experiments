#pragma once

#include <functional>

#define ATMOS_VULKAN_DEBUG 1

#define VULKAN_CHECK_ERR( F, ... )                             \
	igpu::debug::validate(                                     \
		__FILE__,                                              \
		__LINE__,                                              \
		__func__,                                              \
		#F "(" #__VA_ARGS__ ")",                               \
		std::function< decltype( F( __VA_ARGS__ ) )() >( [&] { \
			VULKAN_CONSUME_BREAK_();                           \
			return F( __VA_ARGS__ );                           \
		} ) )
#define VULKAN_CONSUME_BREAK_()                       \
	struct EAT                                        \
	{                                                 \
		~EAT()                                        \
		{                                             \
			if ( igpu::debug::consume_debug_break() ) \
			{                                         \
				__debugbreak();                       \
			}                                         \
		}                                             \
	} _;                                              \
	(void)_
#ifndef VMA_ASSERT
#	define VMA_ASSERT( ... ) ASSERT_CONTEXT( __VA_ARGS__ )
#endif

#ifndef VMA_HEAVY_ASSERT
#	define VMA_HEAVY_ASSERT( ... ) ASSERT_CONTEXT( __VA_ARGS__ )
#endif

#ifndef VMA_DEBUG_LOG
#	define VMA_DEBUG_LOG( ... ) \
		LOG_VERBOSE( "VulkanMemoryAllocator: " __VA_ARGS__ )
#endif

#ifndef VMA_RECORDING_ENABLED
#	define VMA_RECORDING_ENABLED 1
#endif

#include <VulkanMemoryAllocator\src\vk_mem_alloc.h>
#include <vulkan.h>

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
	};
}
