#pragma once

#include <exception>
#include <functional>

#define VULKAN_CHECK_ERR(F, ...) igpu::debug::generate_exception(__FILE__, __LINE__ , __func__, #F"("#__VA_ARGS__")",  std::function<decltype(F(__VA_ARGS__))()>([&] { return F(__VA_ARGS__); } ) )

#include <framework/logging/log.h>

#ifndef VMA_ASSERT
#	define VMA_ASSERT(...) ASSERT_CONTEXT(__VA_ARGS__)
#endif

#ifndef VMA_HEAVY_ASSERT
#	define VMA_HEAVY_ASSERT(...) ASSERT_CONTEXT(__VA_ARGS__)
#endif

#if ATMOS_DEBUG

#	ifndef VMA_DEBUG_LOG
#		define VMA_DEBUG_LOG(...) LOG_CONTEXT(DEBUG, "VulkanMemoryAllocator: " __VA_ARGS__)
#	endif

#	ifndef VMA_RECORDING_ENABLED
#		define VMA_RECORDING_ENABLED 1
#	endif

#endif

#	include <vulkan.h>
#	include <VulkanMemoryAllocator\src\vk_mem_alloc.h>

namespace igpu
{
	struct debug
	{
		static std::string stringify_result(VkResult);

		static void generate_exception(const char* file, int line, const char* func, const char* vk, const std::function<void()>& fun);

		static VkResult generate_exception(const char* file, int line, const char* func, const char* vk, const std::function<VkResult()>& fun);

		class vulkan_result_exception: public std::exception
		{
		public:

			vulkan_result_exception(VkResult, const char* desc);
			VkResult res() const;

		private:

			VkResult _res;
		};
	};
}

#define vmaCreateAllocator(                 ...) VULKAN_CHECK_ERR(vmaCreateAllocator                 , __VA_ARGS__)
#define vmaFindMemoryTypeIndex(             ...) VULKAN_CHECK_ERR(vmaFindMemoryTypeIndex             , __VA_ARGS__)
#define vmaFindMemoryTypeIndexForBufferInfo(...) VULKAN_CHECK_ERR(vmaFindMemoryTypeIndexForBufferInfo, __VA_ARGS__)
#define vmaFindMemoryTypeIndexForImageInfo( ...) VULKAN_CHECK_ERR(vmaFindMemoryTypeIndexForImageInfo , __VA_ARGS__)
#define vmaCreatePool(                      ...) VULKAN_CHECK_ERR(vmaCreatePool                      , __VA_ARGS__)
#define vmaCheckPoolCorruption(             ...) VULKAN_CHECK_ERR(vmaCheckPoolCorruption             , __VA_ARGS__)
#define vmaAllocateMemory(                  ...) VULKAN_CHECK_ERR(vmaAllocateMemory                  , __VA_ARGS__)
#define vmaAllocateMemoryPages(             ...) VULKAN_CHECK_ERR(vmaAllocateMemoryPages             , __VA_ARGS__)
#define vmaAllocateMemoryForBuffer(         ...) VULKAN_CHECK_ERR(vmaAllocateMemoryForBuffer         , __VA_ARGS__)
#define vmaAllocateMemoryForImage(          ...) VULKAN_CHECK_ERR(vmaAllocateMemoryForImage          , __VA_ARGS__)
#define vmaResizeAllocation(                ...) VULKAN_CHECK_ERR(vmaResizeAllocation                , __VA_ARGS__)
#define vmaMapMemory(                       ...) VULKAN_CHECK_ERR(vmaMapMemory                       , __VA_ARGS__)
#define vmaCheckCorruption(                 ...) VULKAN_CHECK_ERR(vmaCheckCorruption                 , __VA_ARGS__)
#define vmaDefragmentationBegin(            ...) VULKAN_CHECK_ERR(vmaDefragmentationBegin            , __VA_ARGS__)
#define vmaDefragmentationEnd(              ...) VULKAN_CHECK_ERR(vmaDefragmentationEnd              , __VA_ARGS__)
#define vmaDefragment(                      ...) VULKAN_CHECK_ERR(vmaDefragment                      , __VA_ARGS__)
#define vmaBindBufferMemory(                ...) VULKAN_CHECK_ERR(vmaBindBufferMemory                , __VA_ARGS__)
#define vmaBindImageMemory(                 ...) VULKAN_CHECK_ERR(vmaBindImageMemory                 , __VA_ARGS__)
#define vmaCreateBuffer(                    ...) VULKAN_CHECK_ERR(vmaCreateBuffer                    , __VA_ARGS__)
#define vmaCreateImage(                     ...) VULKAN_CHECK_ERR(vmaCreateImage                     , __VA_ARGS__)

#define vkCreateInstance(                                ...) VULKAN_CHECK_ERR(vkCreateInstance                                , __VA_ARGS__)
#define vkDestroyInstance(                               ...) VULKAN_CHECK_ERR(vkDestroyInstance                               , __VA_ARGS__)
#define vkEnumeratePhysicalDevices(                      ...) VULKAN_CHECK_ERR(vkEnumeratePhysicalDevices                      , __VA_ARGS__)
#define vkGetPhysicalDeviceFeatures(                     ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceFeatures                     , __VA_ARGS__)
#define vkGetPhysicalDeviceFormatProperties(             ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceFormatProperties             , __VA_ARGS__)
#define vkGetPhysicalDeviceImageFormatProperties(        ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceImageFormatProperties        , __VA_ARGS__)
#define vkGetPhysicalDeviceProperties(                   ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceProperties                   , __VA_ARGS__)
#define vkGetPhysicalDeviceQueueFamilyProperties(        ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceQueueFamilyProperties        , __VA_ARGS__)
#define vkGetPhysicalDeviceMemoryProperties(             ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceMemoryProperties             , __VA_ARGS__)
#define vkGetDeviceProcAddr(                             ...) VULKAN_CHECK_ERR(vkGetDeviceProcAddr                             , __VA_ARGS__)
#define vkCreateDevice(                                  ...) VULKAN_CHECK_ERR(vkCreateDevice                                  , __VA_ARGS__)
#define vkDestroyDevice(                                 ...) VULKAN_CHECK_ERR(vkDestroyDevice                                 , __VA_ARGS__)
#define vkEnumerateInstanceExtensionProperties(          ...) VULKAN_CHECK_ERR(vkEnumerateInstanceExtensionProperties          , __VA_ARGS__)
#define vkEnumerateDeviceExtensionProperties(            ...) VULKAN_CHECK_ERR(vkEnumerateDeviceExtensionProperties            , __VA_ARGS__)
#define vkEnumerateInstanceLayerProperties(              ...) VULKAN_CHECK_ERR(vkEnumerateInstanceLayerProperties              , __VA_ARGS__)
#define vkEnumerateDeviceLayerProperties(                ...) VULKAN_CHECK_ERR(vkEnumerateDeviceLayerProperties                , __VA_ARGS__)
#define vkGetDeviceQueue(                                ...) VULKAN_CHECK_ERR(vkGetDeviceQueue                                , __VA_ARGS__)
#define vkQueueSubmit(                                   ...) VULKAN_CHECK_ERR(vkQueueSubmit                                   , __VA_ARGS__)
#define vkQueueWaitIdle(                                 ...) VULKAN_CHECK_ERR(vkQueueWaitIdle                                 , __VA_ARGS__)
#define vkDeviceWaitIdle(                                ...) VULKAN_CHECK_ERR(vkDeviceWaitIdle                                , __VA_ARGS__)
#define vkAllocateMemory(                                ...) VULKAN_CHECK_ERR(vkAllocateMemory                                , __VA_ARGS__)
#define vkFreeMemory(                                    ...) VULKAN_CHECK_ERR(vkFreeMemory                                    , __VA_ARGS__)
#define vkMapMemory(                                     ...) VULKAN_CHECK_ERR(vkMapMemory                                     , __VA_ARGS__)
#define vkUnmapMemory(                                   ...) VULKAN_CHECK_ERR(vkUnmapMemory                                   , __VA_ARGS__)
#define vkFlushMappedMemoryRanges(                       ...) VULKAN_CHECK_ERR(vkFlushMappedMemoryRanges                       , __VA_ARGS__)
#define vkInvalidateMappedMemoryRanges(                  ...) VULKAN_CHECK_ERR(vkInvalidateMappedMemoryRanges                  , __VA_ARGS__)
#define vkGetDeviceMemoryCommitment(                     ...) VULKAN_CHECK_ERR(vkGetDeviceMemoryCommitment                     , __VA_ARGS__)
#define vkBindBufferMemory(                              ...) VULKAN_CHECK_ERR(vkBindBufferMemory                              , __VA_ARGS__)
#define vkBindImageMemory(                               ...) VULKAN_CHECK_ERR(vkBindImageMemory                               , __VA_ARGS__)
#define vkGetBufferMemoryRequirements(                   ...) VULKAN_CHECK_ERR(vkGetBufferMemoryRequirements                   , __VA_ARGS__)
#define vkGetImageMemoryRequirements(                    ...) VULKAN_CHECK_ERR(vkGetImageMemoryRequirements                    , __VA_ARGS__)
#define vkGetImageSparseMemoryRequirements(              ...) VULKAN_CHECK_ERR(vkGetImageSparseMemoryRequirements              , __VA_ARGS__)
#define vkGetPhysicalDeviceSparseImageFormatProperties(  ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceSparseImageFormatProperties  , __VA_ARGS__)
#define vkQueueBindSparse(                               ...) VULKAN_CHECK_ERR(vkQueueBindSparse                               , __VA_ARGS__)
#define vkCreateFence(                                   ...) VULKAN_CHECK_ERR(vkCreateFence                                   , __VA_ARGS__)
#define vkDestroyFence(                                  ...) VULKAN_CHECK_ERR(vkDestroyFence                                  , __VA_ARGS__)
#define vkResetFences(                                   ...) VULKAN_CHECK_ERR(vkResetFences                                   , __VA_ARGS__)
#define vkGetFenceStatus(                                ...) VULKAN_CHECK_ERR(vkGetFenceStatus                                , __VA_ARGS__)
#define vkWaitForFences(                                 ...) VULKAN_CHECK_ERR(vkWaitForFences                                 , __VA_ARGS__)
#define vkCreateSemaphore(                               ...) VULKAN_CHECK_ERR(vkCreateSemaphore                               , __VA_ARGS__)
#define vkDestroySemaphore(                              ...) VULKAN_CHECK_ERR(vkDestroySemaphore                              , __VA_ARGS__)
#define vkCreateEvent(                                   ...) VULKAN_CHECK_ERR(vkCreateEvent                                   , __VA_ARGS__)
#define vkDestroyEvent(                                  ...) VULKAN_CHECK_ERR(vkDestroyEvent                                  , __VA_ARGS__)
#define vkGetEventStatus(                                ...) VULKAN_CHECK_ERR(vkGetEventStatus                                , __VA_ARGS__)
#define vkSetEvent(                                      ...) VULKAN_CHECK_ERR(vkSetEvent                                      , __VA_ARGS__)
#define vkResetEvent(                                    ...) VULKAN_CHECK_ERR(vkResetEvent                                    , __VA_ARGS__)
#define vkCreateQueryPool(                               ...) VULKAN_CHECK_ERR(vkCreateQueryPool                               , __VA_ARGS__)
#define vkDestroyQueryPool(                              ...) VULKAN_CHECK_ERR(vkDestroyQueryPool                              , __VA_ARGS__)
#define vkGetQueryPoolResults(                           ...) VULKAN_CHECK_ERR(vkGetQueryPoolResults                           , __VA_ARGS__)
#define vkCreateBuffer(                                  ...) VULKAN_CHECK_ERR(vkCreateBuffer                                  , __VA_ARGS__)
#define vkDestroyBuffer(                                 ...) VULKAN_CHECK_ERR(vkDestroyBuffer                                 , __VA_ARGS__)
#define vkCreateBufferView(                              ...) VULKAN_CHECK_ERR(vkCreateBufferView                              , __VA_ARGS__)
#define vkDestroyBufferView(                             ...) VULKAN_CHECK_ERR(vkDestroyBufferView                             , __VA_ARGS__)
#define vkCreateImage(                                   ...) VULKAN_CHECK_ERR(vkCreateImage                                   , __VA_ARGS__)
#define vkDestroyImage(                                  ...) VULKAN_CHECK_ERR(vkDestroyImage                                  , __VA_ARGS__)
#define vkGetImageSubbufferLayout(                     ...) VULKAN_CHECK_ERR(vkGetImageSubbufferLayout                     , __VA_ARGS__)
#define vkCreateImageView(                               ...) VULKAN_CHECK_ERR(vkCreateImageView                               , __VA_ARGS__)
#define vkDestroyImageView(                              ...) VULKAN_CHECK_ERR(vkDestroyImageView                              , __VA_ARGS__)
#define vkCreateShaderModule(                            ...) VULKAN_CHECK_ERR(vkCreateShaderModule                            , __VA_ARGS__)
#define vkDestroyShaderModule(                           ...) VULKAN_CHECK_ERR(vkDestroyShaderModule                           , __VA_ARGS__)
#define vkCreatePipelineCache(                           ...) VULKAN_CHECK_ERR(vkCreatePipelineCache                           , __VA_ARGS__)
#define vkDestroyPipelineCache(                          ...) VULKAN_CHECK_ERR(vkDestroyPipelineCache                          , __VA_ARGS__)
#define vkGetPipelineCacheData(                          ...) VULKAN_CHECK_ERR(vkGetPipelineCacheData                          , __VA_ARGS__)
#define vkMergePipelineCaches(                           ...) VULKAN_CHECK_ERR(vkMergePipelineCaches                           , __VA_ARGS__)
#define vkCreateGraphicsPipelines(                       ...) VULKAN_CHECK_ERR(vkCreateGraphicsPipelines                       , __VA_ARGS__)
#define vkCreateComputePipelines(                        ...) VULKAN_CHECK_ERR(vkCreateComputePipelines                        , __VA_ARGS__)
#define vkDestroyPipeline(                               ...) VULKAN_CHECK_ERR(vkDestroyPipeline                               , __VA_ARGS__)
#define vkCreatePipelineLayout(                          ...) VULKAN_CHECK_ERR(vkCreatePipelineLayout                          , __VA_ARGS__)
#define vkDestroyPipelineLayout(                         ...) VULKAN_CHECK_ERR(vkDestroyPipelineLayout                         , __VA_ARGS__)
#define vkCreateSampler(                                 ...) VULKAN_CHECK_ERR(vkCreateSampler                                 , __VA_ARGS__)
#define vkDestroySampler(                                ...) VULKAN_CHECK_ERR(vkDestroySampler                                , __VA_ARGS__)
#define vkCreateDescriptorSetLayout(                     ...) VULKAN_CHECK_ERR(vkCreateDescriptorSetLayout                     , __VA_ARGS__)
#define vkDestroyDescriptorSetLayout(                    ...) VULKAN_CHECK_ERR(vkDestroyDescriptorSetLayout                    , __VA_ARGS__)
#define vkCreateDescriptorPool(                          ...) VULKAN_CHECK_ERR(vkCreateDescriptorPool                          , __VA_ARGS__)
#define vkDestroyDescriptorPool(                         ...) VULKAN_CHECK_ERR(vkDestroyDescriptorPool                         , __VA_ARGS__)
#define vkResetDescriptorPool(                           ...) VULKAN_CHECK_ERR(vkResetDescriptorPool                           , __VA_ARGS__)
#define vkAllocateDescriptorSets(                        ...) VULKAN_CHECK_ERR(vkAllocateDescriptorSets                        , __VA_ARGS__)
#define vkFreeDescriptorSets(                            ...) VULKAN_CHECK_ERR(vkFreeDescriptorSets                            , __VA_ARGS__)
#define vkUpdateDescriptorSets(                          ...) VULKAN_CHECK_ERR(vkUpdateDescriptorSets                          , __VA_ARGS__)
#define vkCreateFramebuffer(                             ...) VULKAN_CHECK_ERR(vkCreateFramebuffer                             , __VA_ARGS__)
#define vkDestroyFramebuffer(                            ...) VULKAN_CHECK_ERR(vkDestroyFramebuffer                            , __VA_ARGS__)
#define vkCreateRenderPass(                              ...) VULKAN_CHECK_ERR(vkCreateRenderPass                              , __VA_ARGS__)
#define vkDestroyRenderPass(                             ...) VULKAN_CHECK_ERR(vkDestroyRenderPass                             , __VA_ARGS__)
#define vkGetRenderAreaGranularity(                      ...) VULKAN_CHECK_ERR(vkGetRenderAreaGranularity                      , __VA_ARGS__)
#define vkCreateCommandPool(                             ...) VULKAN_CHECK_ERR(vkCreateCommandPool                             , __VA_ARGS__)
#define vkDestroyCommandPool(                            ...) VULKAN_CHECK_ERR(vkDestroyCommandPool                            , __VA_ARGS__)
#define vkResetCommandPool(                              ...) VULKAN_CHECK_ERR(vkResetCommandPool                              , __VA_ARGS__)
#define vkAllocateCommandBuffers(                        ...) VULKAN_CHECK_ERR(vkAllocateCommandBuffers                        , __VA_ARGS__)
#define vkFreeCommandBuffers(                            ...) VULKAN_CHECK_ERR(vkFreeCommandBuffers                            , __VA_ARGS__)
#define vkBeginCommandBuffer(                            ...) VULKAN_CHECK_ERR(vkBeginCommandBuffer                            , __VA_ARGS__)
#define vkEndCommandBuffer(                              ...) VULKAN_CHECK_ERR(vkEndCommandBuffer                              , __VA_ARGS__)
#define vkResetCommandBuffer(                            ...) VULKAN_CHECK_ERR(vkResetCommandBuffer                            , __VA_ARGS__)
#define vkCmdBindPipeline(                               ...) VULKAN_CHECK_ERR(vkCmdBindPipeline                               , __VA_ARGS__)
#define vkCmdSetViewport(                                ...) VULKAN_CHECK_ERR(vkCmdSetViewport                                , __VA_ARGS__)
#define vkCmdSetScissor(                                 ...) VULKAN_CHECK_ERR(vkCmdSetScissor                                 , __VA_ARGS__)
#define vkCmdSetLineWidth(                               ...) VULKAN_CHECK_ERR(vkCmdSetLineWidth                               , __VA_ARGS__)
#define vkCmdSetDepthBias(                               ...) VULKAN_CHECK_ERR(vkCmdSetDepthBias                               , __VA_ARGS__)
#define vkCmdSetBlendConstants(                          ...) VULKAN_CHECK_ERR(vkCmdSetBlendConstants                          , __VA_ARGS__)
#define vkCmdSetDepthBounds(                             ...) VULKAN_CHECK_ERR(vkCmdSetDepthBounds                             , __VA_ARGS__)
#define vkCmdSetStencilCompareMask(                      ...) VULKAN_CHECK_ERR(vkCmdSetStencilCompareMask                      , __VA_ARGS__)
#define vkCmdSetStencilWriteMask(                        ...) VULKAN_CHECK_ERR(vkCmdSetStencilWriteMask                        , __VA_ARGS__)
#define vkCmdSetStencilReference(                        ...) VULKAN_CHECK_ERR(vkCmdSetStencilReference                        , __VA_ARGS__)
#define vkCmdBindDescriptorSets(                         ...) VULKAN_CHECK_ERR(vkCmdBindDescriptorSets                         , __VA_ARGS__)
#define vkCmdBindIndexBuffer(                            ...) VULKAN_CHECK_ERR(vkCmdBindIndexBuffer                            , __VA_ARGS__)
#define vkCmdBindVertexBuffers(                          ...) VULKAN_CHECK_ERR(vkCmdBindVertexBuffers                          , __VA_ARGS__)
#define vkCmdDraw(                                       ...) VULKAN_CHECK_ERR(vkCmdDraw                                       , __VA_ARGS__)
#define vkCmdDrawIndexed(                                ...) VULKAN_CHECK_ERR(vkCmdDrawIndexed                                , __VA_ARGS__)
#define vkCmdDrawIndirect(                               ...) VULKAN_CHECK_ERR(vkCmdDrawIndirect                               , __VA_ARGS__)
#define vkCmdDrawIndexedIndirect(                        ...) VULKAN_CHECK_ERR(vkCmdDrawIndexedIndirect                        , __VA_ARGS__)
#define vkCmdDispatch(                                   ...) VULKAN_CHECK_ERR(vkCmdDispatch                                   , __VA_ARGS__)
#define vkCmdDispatchIndirect(                           ...) VULKAN_CHECK_ERR(vkCmdDispatchIndirect                           , __VA_ARGS__)
#define vkCmdCopyBuffer(                                 ...) VULKAN_CHECK_ERR(vkCmdCopyBuffer                                 , __VA_ARGS__)
#define vkCmdCopyImage(                                  ...) VULKAN_CHECK_ERR(vkCmdCopyImage                                  , __VA_ARGS__)
#define vkCmdBlitImage(                                  ...) VULKAN_CHECK_ERR(vkCmdBlitImage                                  , __VA_ARGS__)
#define vkCmdCopyBufferToImage(                          ...) VULKAN_CHECK_ERR(vkCmdCopyBufferToImage                          , __VA_ARGS__)
#define vkCmdCopyImageToBuffer(                          ...) VULKAN_CHECK_ERR(vkCmdCopyImageToBuffer                          , __VA_ARGS__)
#define vkCmdUpdateBuffer(                               ...) VULKAN_CHECK_ERR(vkCmdUpdateBuffer                               , __VA_ARGS__)
#define vkCmdFillBuffer(                                 ...) VULKAN_CHECK_ERR(vkCmdFillBuffer                                 , __VA_ARGS__)
#define vkCmdClearColorImage(                            ...) VULKAN_CHECK_ERR(vkCmdClearColorImage                            , __VA_ARGS__)
#define vkCmdClearDepthStencilImage(                     ...) VULKAN_CHECK_ERR(vkCmdClearDepthStencilImage                     , __VA_ARGS__)
#define vkCmdClearAttachments(                           ...) VULKAN_CHECK_ERR(vkCmdClearAttachments                           , __VA_ARGS__)
#define vkCmdResolveImage(                               ...) VULKAN_CHECK_ERR(vkCmdResolveImage                               , __VA_ARGS__)
#define vkCmdSetEvent(                                   ...) VULKAN_CHECK_ERR(vkCmdSetEvent                                   , __VA_ARGS__)
#define vkCmdResetEvent(                                 ...) VULKAN_CHECK_ERR(vkCmdResetEvent                                 , __VA_ARGS__)
#define vkCmdWaitEvents(                                 ...) VULKAN_CHECK_ERR(vkCmdWaitEvents                                 , __VA_ARGS__)
#define vkCmdPipelineBarrier(                            ...) VULKAN_CHECK_ERR(vkCmdPipelineBarrier                            , __VA_ARGS__)
#define vkCmdBeginQuery(                                 ...) VULKAN_CHECK_ERR(vkCmdBeginQuery                                 , __VA_ARGS__)
#define vkCmdEndQuery(                                   ...) VULKAN_CHECK_ERR(vkCmdEndQuery                                   , __VA_ARGS__)
#define vkCmdResetQueryPool(                             ...) VULKAN_CHECK_ERR(vkCmdResetQueryPool                             , __VA_ARGS__)
#define vkCmdWriteTimestamp(                             ...) VULKAN_CHECK_ERR(vkCmdWriteTimestamp                             , __VA_ARGS__)
#define vkCmdCopyQueryPoolResults(                       ...) VULKAN_CHECK_ERR(vkCmdCopyQueryPoolResults                       , __VA_ARGS__)
#define vkCmdPushConstants(                              ...) VULKAN_CHECK_ERR(vkCmdPushConstants                              , __VA_ARGS__)
#define vkCmdBeginRenderPass(                            ...) VULKAN_CHECK_ERR(vkCmdBeginRenderPass                            , __VA_ARGS__)
#define vkCmdNextSubpass(                                ...) VULKAN_CHECK_ERR(vkCmdNextSubpass                                , __VA_ARGS__)
#define vkCmdEndRenderPass(                              ...) VULKAN_CHECK_ERR(vkCmdEndRenderPass                              , __VA_ARGS__)
#define vkCmdExecuteCommands(                            ...) VULKAN_CHECK_ERR(vkCmdExecuteCommands                            , __VA_ARGS__)
#define vkDestroySurfaceKHR(                             ...) VULKAN_CHECK_ERR(vkDestroySurfaceKHR                             , __VA_ARGS__)
#define vkGetPhysicalDeviceSurfaceSupportKHR(            ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceSurfaceSupportKHR            , __VA_ARGS__)
#define vkGetPhysicalDeviceSurfaceCapabilitiesKHR(       ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceSurfaceCapabilitiesKHR       , __VA_ARGS__)
#define vkGetPhysicalDeviceSurfaceFormatsKHR(            ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceSurfaceFormatsKHR            , __VA_ARGS__)
#define vkGetPhysicalDeviceSurfacePresentModesKHR(       ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceSurfacePresentModesKHR       , __VA_ARGS__)
#define vkCreateSwapchainKHR(                            ...) VULKAN_CHECK_ERR(vkCreateSwapchainKHR                            , __VA_ARGS__)
#define vkDestroySwapchainKHR(                           ...) VULKAN_CHECK_ERR(vkDestroySwapchainKHR                           , __VA_ARGS__)
#define vkGetSwapchainImagesKHR(                         ...) VULKAN_CHECK_ERR(vkGetSwapchainImagesKHR                         , __VA_ARGS__)
#define vkAcquireNextImageKHR(                           ...) VULKAN_CHECK_ERR(vkAcquireNextImageKHR                           , __VA_ARGS__)
#define vkQueuePresentKHR(                               ...) VULKAN_CHECK_ERR(vkQueuePresentKHR                               , __VA_ARGS__)
#define vkGetPhysicalDeviceDisplayPropertiesKHR(         ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceDisplayPropertiesKHR         , __VA_ARGS__)
#define vkGetPhysicalDeviceDisplayPlanePropertiesKHR(    ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceDisplayPlanePropertiesKHR    , __VA_ARGS__)
#define vkGetDisplayPlaneSupportedDisplaysKHR(           ...) VULKAN_CHECK_ERR(vkGetDisplayPlaneSupportedDisplaysKHR           , __VA_ARGS__)
#define vkGetDisplayModePropertiesKHR(                   ...) VULKAN_CHECK_ERR(vkGetDisplayModePropertiesKHR                   , __VA_ARGS__)
#define vkCreateDisplayModeKHR(                          ...) VULKAN_CHECK_ERR(vkCreateDisplayModeKHR                          , __VA_ARGS__)
#define vkGetDisplayPlaneCapabilitiesKHR(                ...) VULKAN_CHECK_ERR(vkGetDisplayPlaneCapabilitiesKHR                , __VA_ARGS__)
#define vkCreateDisplayPlaneSurfaceKHR(                  ...) VULKAN_CHECK_ERR(vkCreateDisplayPlaneSurfaceKHR                  , __VA_ARGS__)
#define vkCreateSharedSwapchainsKHR(                     ...) VULKAN_CHECK_ERR(vkCreateSharedSwapchainsKHR                     , __VA_ARGS__)
#ifdef VK_USE_PLATFORM_XLIB_KHR
#define vkCreateXlibSurfaceKHR(                          ...) VULKAN_CHECK_ERR(vkCreateXlibSurfaceKHR                          , __VA_ARGS__)
#define vkGetPhysicalDeviceXlibPresentationSupportKHR(   ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceXlibPresentationSupportKHR   , __VA_ARGS__)
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
#define vkCreateXcbSurfaceKHR(                           ...) VULKAN_CHECK_ERR(vkCreateXcbSurfaceKHR                           , __VA_ARGS__)
#define vkGetPhysicalDeviceXcbPresentationSupportKHR(    ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceXcbPresentationSupportKHR    , __VA_ARGS__)
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
#define vkCreateWaylandSurfaceKHR(                       ...) VULKAN_CHECK_ERR(vkCreateWaylandSurfaceKHR                       , __VA_ARGS__)
#define vkGetPhysicalDeviceWaylandPresentationSupportKHR(...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceWaylandPresentationSupportKHR, __VA_ARGS__)
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
#define vkCreateAndroidSurfaceKHR(                       ...) VULKAN_CHECK_ERR(vkCreateAndroidSurfaceKHR                       , __VA_ARGS__)
#endif
#ifdef VK_USE_PLATFORM_WIN32_KHR
#define vkCreateWin32SurfaceKHR(                         ...) VULKAN_CHECK_ERR(vkCreateWin32SurfaceKHR                         , __VA_ARGS__)
#define vkGetPhysicalDeviceWin32PresentationSupportKHR(  ...) VULKAN_CHECK_ERR(vkGetPhysicalDeviceWin32PresentationSupportKHR  , __VA_ARGS__)
#endif
#define vkCreateDebugReportCallbackEXT(                  ...) VULKAN_CHECK_ERR(vkCreateDebugReportCallbackEXT                  , __VA_ARGS__)
#define vkDestroyDebugReportCallbackEXT(                 ...) VULKAN_CHECK_ERR(vkDestroyDebugReportCallbackEXT                 , __VA_ARGS__)
#define vkDebugReportMessageEXT(                         ...) VULKAN_CHECK_ERR(vkDebugReportMessageEXT                         , __VA_ARGS__)