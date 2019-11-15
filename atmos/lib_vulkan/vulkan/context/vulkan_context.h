
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

		std::unique_ptr< draw_target > make_draw_target(
			const draw_target::config& ) override;

		std::unique_ptr< render_buffer > make_render_buffer(
			const render_buffer::config& ) override;

		std::unique_ptr< render_texture2d > make_render_texture2d(
			const render_texture2d::config& ) override;

		std::unique_ptr< depth_buffer > make_depth_buffer(
			const depth_buffer::config& ) override;

		std::unique_ptr< depth_texture2d > make_depth_texture2d(
			const depth_texture2d::config& ) override;

		std::unique_ptr< program > make_program(
			const program::config& ) override;

		std::unique_ptr< vertex_shader > make_vertex_shader() override;

		std::unique_ptr< fragment_shader > make_fragment_shader() override;

		std::unique_ptr< render_states > make_render_states(
			const render_states::config& ) override;

		std::unique_ptr< geometry > make_geometry(
			const geometry::config& ) override;

		std::unique_ptr< vertex_buffer > make_vertex_buffer(
			const vertex_buffer::config& ) override;

		std::unique_ptr< index_buffer > make_index_buffer(
			const index_buffer::config& ) override;

		std::unique_ptr< compute_buffer > make_compute_buffer(
			const compute_buffer::config& ) override;

		std::unique_ptr< texture2d > make_texture(
			const texture2d::config& ) override;

		std::unique_ptr< primitives > make_primitives(
			const primitives::config& ) override;

		std::unique_ptr< opaque_batch > make_opaque_batch(
			const opaque_batch::config& ) override;

		std::unique_ptr< transparent_batch > make_transparent_batch(
			const transparent_batch::config& ) override;

		const vulkan_window& window() const override;

		const vulkan_back_buffer& back_buffer() const override;

		~vulkan_context();

		// todo: delete these methods and the need for their functionality
		VkInstance instance();
		VkPhysicalDevice physical_device();
		VkDevice device();
		vulkan_synchronization& synchronization();
		void resize_back_buffer( const glm::ivec2& screen_res );

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
