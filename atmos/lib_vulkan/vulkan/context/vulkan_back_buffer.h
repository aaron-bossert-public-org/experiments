
#pragma once

#include <vector>

#include <vulkan/defines/vulkan_includes.h>

#include <igpu/context/back_buffer.h>

namespace igpu
{
	class vulkan_depth_buffer;
	class vulkan_color_buffer;

	class vulkan_back_buffer : public back_buffer
	{
	public:

		struct config : back_buffer::config {};

		const config& cfg() const override;

		static std::unique_ptr<vulkan_back_buffer> make(const config&);

	private:

		vulkan_back_buffer(const config&);

	private:

		const config _cfg;

		VkSurfaceCapabilitiesKHR _capabilities;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		VkSampleCountFlagBits _msaa_samples = VK_SAMPLE_COUNT_1_BIT;
		VkSurfaceKHR _surface;
		VkSwapchainKHR _swap_chain;
		std::vector<VkImage> _swap_chain_images;
		VkFormat _swap_chain_image_format;
		VkExtent2D _swap_chain_extent;
		std::vector<VkImageView> _swap_chain_image_views;
		std::vector<VkFramebuffer> _swap_chain_framebuffers;
	};
}
