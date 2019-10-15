
#pragma once

#include <igpu/utility/utility_types.h>
#include <igpu/context/context.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/material/vulkan_render_states.h>
#include <vulkan/window/vulkan_window.h>

#include <framework/perf/metrics.h>

namespace igpu
{
    class vulkan_buffer_mediator;
	class vulkan_program;
    class vulkan_vertex_buffer;
    class vulkan_index_buffer;
	class vulkan_render_states;
    class vulkan_geometry;
    
    class vulkan_context : public context
    {
    public:

		struct config : context::config
		{
			std::shared_ptr<vulkan_window> window;
		};

		static std::unique_ptr<vulkan_context> make(
			const config&);

		const config& cfg() const override;

		std::unique_ptr<program> make_program(
			const buffer_view<uint8_t>& vertex_code,
			const buffer_view<uint8_t>& pixel_code) override;

		std::unique_ptr<geometry> make_geometry(
			const geometry::config&) override;

		std::unique_ptr<vertex_buffer> make_vertex_buffer(
			const vertex_buffer::config&) override;

		std::unique_ptr<index_buffer> make_index_buffer(
			const index_buffer::config&) override;

		std::unique_ptr<compute_buffer> make_compute_buffer(
			const compute_buffer::config&) override;
		
		const igpu::batch_constraints& batch_constraints() const override;

		const igpu::material_constraints& material_constraints() const override;

		const igpu::vertex_constraints& vertex_constraints() const override;

		const igpu::vulkan_window& window() const override;

        ~vulkan_context();

		// todo: delete these accessors
		VkInstance instance();
		VkSurfaceKHR surface_khr();
		VkPhysicalDevice physical_device();
		VkDevice device();
		const std::shared_ptr<vulkan_buffer_mediator>& buffer_mediator();

    protected:
        
		vulkan_context(
			const config&,
			VkInstance,
			VkSurfaceKHR,
			VkPhysicalDevice,
			VkDevice,
			VkDebugUtilsMessengerEXT,
			VkSampleCountFlagBits,
			const std::shared_ptr<vulkan_buffer_mediator>&);
        
    private:
		const config _cfg;

		VkInstance _instance = nullptr;
		VkSurfaceKHR _surface_khr = nullptr;
		VkPhysicalDevice _physical_device = nullptr;
		VkDevice _device = nullptr;
		VkDebugUtilsMessengerEXT _debug_messenger = nullptr;

		VkSampleCountFlagBits _max_usable_sample_count = VK_SAMPLE_COUNT_1_BIT;
		
		igpu::batch_constraints _batch_constraints;
		igpu::material_constraints _material_constraints;
		igpu::vertex_constraints _vertex_constraints;

		std::shared_ptr<vulkan_buffer_mediator> _buffer_mediator;

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
