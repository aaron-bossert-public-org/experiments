
#pragma once

#include <vector>

#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/texture/vulkan_color_buffer.h>
#include <vulkan/texture/vulkan_depth_buffer.h>

#include <igpu/window/back_buffer.h>

namespace igpu
{
	class vulkan_queue;

	class vulkan_back_buffer : public back_buffer
	{
	public:

		struct config : back_buffer::config {

			VkPhysicalDevice physical_device = nullptr;
			VkSurfaceKHR surface = nullptr;
			VkDevice device = nullptr;
			VkColorSpaceKHR color_space;
			std::shared_ptr < vulkan_queue > present_queue;
			std::shared_ptr < vulkan_queue > graphics_queue;
			VkSampleCountFlagBits sample_count;
		};

		const config& cfg() const override;

		const vulkan_color_buffer& color() const override;

		const vulkan_depth_buffer& depth() const override;

		VkSwapchainKHR swap_chain() const;

		VkRenderPass render_pass() const;

		const std::vector<VkFramebuffer>& framebuffers() const;

		static std::unique_ptr<vulkan_back_buffer> make(const config&);

		~vulkan_back_buffer();

	private:

		vulkan_back_buffer(
			const config&,
			VkSwapchainKHR,
			VkRenderPass,
			const std::vector<VkImage>&,
			const std::vector<VkImageView>&,
			const std::vector<VkFramebuffer>&,
			const std::shared_ptr<vulkan_color_buffer>&,
			const std::shared_ptr<vulkan_depth_buffer>&);

	private:

		const config _cfg;

		VkSwapchainKHR _swap_chain;
		VkRenderPass _render_pass;

		std::vector<VkImage> _images;
		std::vector<VkImageView> _image_views;
		std::vector<VkFramebuffer> _framebuffers;

		std::shared_ptr < vulkan_color_buffer > _color;
		std::shared_ptr < vulkan_depth_buffer > _depth;
	};
}
