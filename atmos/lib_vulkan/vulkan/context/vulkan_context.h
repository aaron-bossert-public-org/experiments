
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/window/vulkan_back_buffer.h"
#include "vulkan/window/vulkan_window.h"

#include "igpu/context/context.h"
#include "igpu/utility/utility_types.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class vulkan_synchronization;
	class vulkan_pipeline_cache;

	class vulkan_context : public context
	{
	public:
		struct config : context::config
		{
			struct vulkan
			{
				VkPhysicalDeviceProperties physical_device_properties;
			};

			vulkan vk;
		};

		static std::unique_ptr< vulkan_context > make(
			const context::config&,
			const glm::ivec2& screen_res );

		const config& cfg() const override;

		void resize_back_buffer( const glm::ivec2& screen_res );

		~vulkan_context();

	private:
		std::unique_ptr< graphics_pipeline > make(
			const graphics_pipeline ::config& ) override;

		std::unique_ptr< draw_target > make(
			const draw_target::config& ) override;

		std::unique_ptr< render_buffer > make(
			const render_buffer::config& ) override;

		std::unique_ptr< render_texture2d > make(
			const render_texture2d::config& ) override;

		std::unique_ptr< depth_buffer > make(
			const depth_buffer::config& ) override;

		std::unique_ptr< depth_texture2d > make(
			const depth_texture2d::config& ) override;

		std::unique_ptr< program > make( const program::config& ) override;

		std::unique_ptr< vertex_shader > make(
			const vertex_shader::config& ) override;

		std::unique_ptr< fragment_shader > make(
			const fragment_shader::config& ) override;

		std::unique_ptr< render_states > make(
			const render_states::config& ) override;

		std::unique_ptr< geometry > make( const geometry::config& ) override;

		std::unique_ptr< vertex_buffer > make(
			const vertex_buffer::config& ) override;

		std::unique_ptr< index_buffer > make(
			const index_buffer::config& ) override;

		std::unique_ptr< compute_buffer > make(
			const compute_buffer::config& ) override;

		std::unique_ptr< texture2d > make( const texture2d::config& ) override;

		std::unique_ptr< primitives > make(
			const primitives::config& ) override;

		std::unique_ptr< opaque_batch > make(
			const opaque_batch::config& ) override;

		std::unique_ptr< transparent_batch > make(
			const transparent_batch::config& ) override;

		const vulkan_window& window() const override;

		const vulkan_back_buffer& back_buffer() const override;

	protected:
		vulkan_context(
			const config&,
			VkInstance,
			VkDebugUtilsMessengerEXT,
			VkPhysicalDevice,
			VkDevice,
			const std::shared_ptr< vulkan_queue >& present_queue,
			const std::shared_ptr< vulkan_queue >& graphics_queue,
			const std::shared_ptr< vulkan_queue >& compute_queue,
			const std::shared_ptr< vulkan_queue >& transfer_queue,
			const std::shared_ptr< vulkan_pipeline_cache >&,
			const std::shared_ptr< vulkan_synchronization >&,
			std::unique_ptr< vulkan_window >,
			std::unique_ptr< vulkan_back_buffer > );

	private:
		const config _cfg;

		struct auto_destroy
		{
			~auto_destroy();

			VkInstance instance = nullptr;
			VkDebugUtilsMessengerEXT debug_messenger = nullptr;
			VkPhysicalDevice physical_device = nullptr;
			VkDevice device = nullptr;
		} _state;

		std::shared_ptr< vulkan_queue > _present_queue;
		std::shared_ptr< vulkan_queue > _graphics_queue;
		std::shared_ptr< vulkan_queue > _compute_queue;
		std::shared_ptr< vulkan_queue > _transfer_queue;

		std::shared_ptr< vulkan_pipeline_cache > _pipeline_cache;

		std::shared_ptr< vulkan_synchronization > _synchronization;

		std::unique_ptr< vulkan_window > _window;
		std::unique_ptr< vulkan_back_buffer > _back_buffer;

#if ATMOS_PERFORMANCE_TRACKING
		perf::metric _renderstate_switch_metric;
		perf::metric _draw_target_clears_metric;
		perf::metric _draw_target_switch_metric;
		perf::metric _shader_switch_metric;
		perf::metric _geometry_switch_metric;
		perf::metric _draw_call_metric;
		perf::metric _polycount_metric;
#endif
	};
}
