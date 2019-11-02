
#pragma once

#include <vector>

#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/texture/vulkan_render_buffer.h>
#include <vulkan/texture/vulkan_depth_buffer.h>

#include <igpu/window/back_buffer.h>

namespace igpu
{
	class vulkan_queue;

	class vulkan_back_buffer : public back_buffer
	{
	public:

		struct config : back_buffer::config 
		{
			struct vulkan
			{
				VkPhysicalDevice physical_device = nullptr;
				VkSurfaceKHR surface = nullptr;
				VkDevice device = nullptr;
				VkColorSpaceKHR color_space;
				uint32_t present_queue_family = 0;
				uint32_t graphics_queue_family = 0;
				VkSampleCountFlagBits sample_count;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		const vulkan_render_buffer& color() const override;

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
			std::unique_ptr<vulkan_render_buffer>,
			std::unique_ptr<vulkan_depth_buffer>);

	private:

		const config _cfg;

		VkSwapchainKHR _swap_chain;
		VkRenderPass _render_pass;

		std::vector<VkImage> _images;
		std::vector<VkImageView> _image_views;
		std::vector<VkFramebuffer> _framebuffers;

		std::unique_ptr < vulkan_render_buffer > _color;
		std::unique_ptr < vulkan_depth_buffer > _depth;
	};
}
