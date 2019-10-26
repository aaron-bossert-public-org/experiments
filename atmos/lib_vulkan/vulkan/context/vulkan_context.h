
#pragma once

#include <igpu/utility/utility_types.h>
#include <igpu/context/context.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/window/vulkan_back_buffer.h>
#include <vulkan/window/vulkan_window.h>

#include <framework/perf/metrics.h>

namespace igpu
{
    class vulkan_buffer_mediator;
	
    class vulkan_context : public context
    {
    public:

		struct config : context::config
		{
		};

		static std::unique_ptr<vulkan_context> make(
			const config&,
			const glm::ivec2& screen_res);

		const config& cfg() const override;

		std::unique_ptr<program> make_program(
			const program::config&) override;

		std::unique_ptr<vertex_shader> make_vertex_shader(
			const vertex_shader::config&) override;

		std::unique_ptr<fragment_shader> make_fragment_shader(
			const fragment_shader::config&) override;

		std::unique_ptr<geometry> make_geometry(
			const geometry::config&) override;

		std::unique_ptr<vertex_buffer> make_vertex_buffer(
			const vertex_buffer::config&) override;

		std::unique_ptr<index_buffer> make_index_buffer(
			const index_buffer::config&) override;

		std::unique_ptr<compute_buffer> make_compute_buffer(
			const compute_buffer::config&) override;

		std::unique_ptr<texture2d> make_texture(
			const texture2d::config&) override;
		
		const igpu::batch_constraints& batch_constraints() const override;

		const igpu::material_constraints& material_constraints() const override;

		const igpu::vertex_constraints& vertex_constraints() const override;

		const vulkan_window& window() const override;

		const vulkan_back_buffer& back_buffer() const override;


        ~vulkan_context();

		// todo: delete these methods and the need for their functionality
		VkInstance instance();
		VkPhysicalDevice physical_device();
		VkDevice device();
		vulkan_buffer_mediator& buffer_mediator();
		void resize_back_buffer(const glm::ivec2& screen_res);

    protected:
        
		vulkan_context(
			const config&,
			VkInstance,
			VkDebugUtilsMessengerEXT,
			VkPhysicalDevice,
			VkDevice,
			const std::shared_ptr<vulkan_queue>& present_queue,
			const std::shared_ptr<vulkan_queue>& graphics_queue,
			const std::shared_ptr<vulkan_queue>& compute_queue,
			const std::shared_ptr<vulkan_queue>& transfer_queue, 
			const std::shared_ptr<vulkan_buffer_mediator>&,
			std::unique_ptr<vulkan_window>,
			std::unique_ptr<vulkan_back_buffer>);
        
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

		std::shared_ptr<vulkan_queue> _present_queue;
		std::shared_ptr<vulkan_queue> _graphics_queue;
		std::shared_ptr<vulkan_queue> _compute_queue;
		std::shared_ptr<vulkan_queue> _transfer_queue;

		std::shared_ptr<vulkan_buffer_mediator> _buffer_mediator;
		std::unique_ptr<vulkan_window> _window;
		std::unique_ptr<vulkan_back_buffer> _back_buffer;

		igpu::batch_constraints _batch_constraints;
		igpu::material_constraints _material_constraints;
		igpu::vertex_constraints _vertex_constraints;


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
