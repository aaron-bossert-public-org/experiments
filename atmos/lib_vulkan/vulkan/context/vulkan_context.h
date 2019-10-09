
#pragma once

#include <igpu/utility/utility_types.h>
#include <igpu/context/context.h>
#include <vulkan/material/vulkan_render_states.h>

#include <framework/perf/metrics.h>

namespace igpu
{
    class vulkan_command_composer;
	class vulkan_program;
    class vulkan_vertex_resource;
    class vulkan_index_resource;
	class vulkan_render_states;
    class vulkan_geometry;
    class vulkan_window;
    
    class vulkan_context : public context
    {
    public:

		static std::unique_ptr<vulkan_context> make(const config&);

		std::unique_ptr<program> make_program(
			const buffer_view<uint8_t>& vertex_code,
			const buffer_view<uint8_t>& pixel_code) override;

		std::unique_ptr<geometry> make_geometry(
			std::string name,
			topology topology,
			size_t element_start,
			size_t element_count,
			std::vector<std::shared_ptr<vertex_resource>>,
			std::shared_ptr<index_resource>) override;

		std::unique_ptr<vertex_resource> make_vertex_resource(
			const vertex_resource::config&) override;

		std::unique_ptr<index_resource> make_index_resource(
			const index_resource::config&) override;

		std::unique_ptr<compute_resource> make_compute_resource(
			const compute_resource::config&) override;
		
		const igpu::batch_constraints& batch_constraints() const override;

		const igpu::material_constraints& material_constraints() const override;

		const igpu::vertex_constraints& vertex_constraints() const override;

		const igpu::window& window() const override;

        ~vulkan_context();

    protected:
        
		vulkan_context(
			const config&,
			std::shared_ptr<vulkan_command_composer>);
        
    private:
		const config _cfg;
		
		igpu::batch_constraints _batch_constraints;
		igpu::material_constraints _material_constraints;
		igpu::vertex_constraints _vertex_constraints;

		const std::shared_ptr<vulkan_command_composer> _command_composer;
		const std::shared_ptr<vulkan_window> _window;

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
