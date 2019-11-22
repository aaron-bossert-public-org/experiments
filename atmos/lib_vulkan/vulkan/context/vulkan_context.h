
#pragma once

#include "vulkan/defines/vulkan_includes.h"

#include "igpu/context/context.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class vulkan_back_buffer;
	class vulkan_barrier_manager;
	class vulkan_managers;
	class vulkan_pipeline_cache;
	class vulkan_queue;
	class vulkan_queue_manager;
	class vulkan_staging_manager;
	class vulkan_window;

	class vulkan_context : public context
	{
	public:
		struct config : context::config
		{
			struct vulkan
			{
				VkInstance instance = nullptr;
				VkDebugUtilsMessengerEXT debug_messenger = nullptr;
				VkPhysicalDevice physical_device = nullptr;
				VkDevice device = nullptr;
				VmaAllocator vma = nullptr;
				VkPhysicalDeviceProperties physical_device_properties;
				VkSampleCountFlagBits sample_count = (VkSampleCountFlagBits)0;
				uint32_t swap_count = 0;
			};

			vulkan vk;
		};
		struct state
		{
			std::shared_ptr< vulkan_window > window;
			std::shared_ptr< vulkan_queue > present_queue;
			std::shared_ptr< vulkan_queue > graphics_queue;
			std::shared_ptr< vulkan_queue > compute_queue;
			std::shared_ptr< vulkan_queue > transfer_queue;
			std::shared_ptr< vulkan_queue_manager > queue_manager;
			std::shared_ptr< vulkan_barrier_manager > barrier_manager;
			std::shared_ptr< vulkan_staging_manager > staging_manager;
			std::shared_ptr< vulkan_managers > managers;
			std::shared_ptr< vulkan_back_buffer > back_buffer;
			std::shared_ptr< vulkan_pipeline_cache > pipeline_cache;

			bool back_buffer_out_of_date = false;
		};

		static std::unique_ptr< vulkan_context > make(
			const context::config&,
			const glm::ivec2& screen_res );

		const config& cfg() const override;

		scoped_ptr< igpu::draw_target > back_buffer() override;

		scoped_ptr< igpu::window > window() const override;

		void back_buffer_out_of_date();

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

	protected:
		vulkan_context( config&&, state&& );

	private:
		state _st;
		const config _cfg;
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
