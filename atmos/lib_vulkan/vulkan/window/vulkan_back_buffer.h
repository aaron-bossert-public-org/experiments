
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_draw_target.h"
#include "vulkan/texture/vulkan_render_buffer.h"

#include <vector>

namespace igpu
{
	class vulkan_context;
	class vulkan_queue;

	class vulkan_back_buffer : public vulkan_draw_target
	{
	public:
		struct config : vulkan_draw_target::config
		{
			struct back_buffer
			{
				vulkan_context* context = nullptr;
				VkPhysicalDevice physical_device = nullptr;
				VkSurfaceKHR surface = nullptr;
				VkColorSpaceKHR color_space;
				uint32_t present_queue_family = 0;
				uint32_t graphics_queue_family = 0;
			};

			back_buffer back_buffer;
		};

		const config& cfg() const override;

		void end_raster() override;

		VkSwapchainKHR swap_chain() const;

		const std::vector< VkFramebuffer >& framebuffers() const;

		static std::unique_ptr< vulkan_back_buffer > make( const config& );

		~vulkan_back_buffer();

	private:
		vulkan_back_buffer(
			const config&,
			VkSwapchainKHR,
			const std::vector< VkImage >&,
			const std::vector< VkImageView >& );

	private:
		const config _cfg;

		VkSwapchainKHR _swap_chain;

		std::vector< VkImage > _images;
		std::vector< VkImageView > _image_views;
		std::vector< VkFramebuffer > _framebuffers;

		std::unique_ptr< vulkan_render_buffer > _color;
		std::unique_ptr< vulkan_depth_buffer > _depth;
	};
}
