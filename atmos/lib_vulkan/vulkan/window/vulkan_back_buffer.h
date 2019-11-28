
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_draw_target.h"
#include "vulkan/texture/vulkan_render_buffer.h"

#include <memory>
#include <vector>

namespace igpu
{
	class vulkan_context;
	class vulkan_queue;
	class vulkan_semaphore;
	class vulkan_command_pool;

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
				VkSurfaceCapabilitiesKHR surface_caps;
				VkColorSpaceKHR color_space;
				bool vsync = true;
			};

			back_buffer back_buffer;
		};

		const config& cfg() const override;

		void begin_raster() override;

		scoped_ptr< vulkan_command_buffer > raster_cmds() override;

		const vulkan_poset_fence& raster_fence() const override;

		void end_raster() override;

		size_t swap_index() const override;

		VkSwapchainKHR swap_chain() const;

		VkFramebuffer framebuffer( size_t swap_index ) const;

		static std::unique_ptr< vulkan_back_buffer > make( const config& );

		~vulkan_back_buffer();

	private:
		struct swap_state
		{
			VkImage image = nullptr;
			VkImageView image_view = nullptr;
			VkFramebuffer framebuffer = nullptr;
			std::shared_ptr< vulkan_command_buffer > raster_cmds;
			std::shared_ptr< vulkan_semaphore > aquire_sem;
			std::shared_ptr< vulkan_semaphore > raster_sem;
			vulkan_poset_fence raster_fence;
		};

		struct state
		{
			VkSwapchainKHR swap_chain;
			std::shared_ptr< vulkan_command_pool > command_pool;
			std::vector< vulkan_back_buffer::swap_state > swap_states;
			uint32_t swap_index = 0;
			uint32_t image_index = 0;
		};

		vulkan_back_buffer( const config&, const state& );

		swap_state& frame_swap_state();

		VkResult do_end_raster();

	private:
		const config _cfg;
		state _st;
	};
}