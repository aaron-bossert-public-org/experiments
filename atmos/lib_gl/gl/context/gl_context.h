
#pragma once
#include <gl/buffer/gl_vertex_attrib_array_tracker.h>
#include <gl/material/gl_render_states.h>
#include <gl/window/gl_back_buffer.h>
#include <gl/window/gl_window.h>

#include <igpu/context/context.h>
#include <igpu/context/batch_constraints.h>
#include <igpu/context/material_constraints.h>
#include <igpu/context/vertex_constraints.h>
#include <igpu/utility/utility_types.h>
#include <framework/perf/metrics.h>

namespace igpu
{
    class batch;
	class gl_back_buffer;
	class gl_draw_target;
	class gl_program;
    class gl_vertex_buffer;
    class gl_index_buffer;
    class gl_geometry;
    class gl_window;
    
    class gl_context : public context
    {
    public:

		static std::unique_ptr<gl_context> make(
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
			const geometry::config& cfg) override;

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

		const gl_window& window() const override;

		const gl_back_buffer& back_buffer() const override;

        void set_back_buffer(const gl_back_buffer*);
        
        void init_render_states();
        
        void set_render_states(const gl_render_states*, bool force = false);
                
        void begin_batch(batch*);
        
        void end_batch(const batch*);

        const batch* active_batch() const;
        
        void set_viewport();
        
        void set_viewport(const utility::viewport&);
        
        const utility::viewport& get_viewport() const;
        
        void set_scissor();
        
        void set_scissor(const utility::scissor& scissor);
        
        void clear(const glm::vec4* color, const float* depth, const uint8_t* stencil);
        
        void set_draw_target(gl_draw_target* target);
        
        void begin_program(gl_program*);

        void end_program(const gl_program*);

		const gl_program* active_program() const;
                
        void begin_geometry(const gl_geometry*);
        
        void end_geometry(const gl_geometry*);
        
        void draw(int32_t start, int32_t count);
        
        void flush();
        
		void active_texture(unsigned stage, unsigned handle, bool force = false);

        void set_color_write_mask( unsigned, bool force = false );
        
        void set_mode( const gl_cull_mode&, bool force = false );
        
        void set_mode( const gl_blend_mode&, bool force = false );
        
        void set_mode( const gl_depth_mode&, bool force = false );
        
        void set_mode( const gl_stencil_mode&, bool force = false );
        
        void set_mode( const gl_packed_states&, bool force = false );
        
        const gl_vertex_attrib_array_tracker& vertex_attrib_array_tracker() const;

        ~gl_context();

    protected:
        
		gl_context(
			const config&,
			std::unique_ptr<gl_window> gl_window);
        
    private:
        
		config _cfg;
		igpu::batch_constraints _batch_constraints;
		igpu::material_constraints _material_constraints;
		igpu::vertex_constraints _vertex_constraints;
		std::unique_ptr<gl_window> _gl_window;
		utility::viewport _current_viewport;
		size_t _batch_drawpass_id = 0;
        batch* _active_batch = nullptr;
        gl_program* _active_program = nullptr;
		gl_vertex_attrib_array_tracker _vertex_attrib_array_tracker;
        gl_packed_states _packed_states;
        unsigned _active_texture_stage = 0;
		struct
		{
			const gl_geometry* geometry = nullptr;
			unsigned gl_topology = 0;
			unsigned gl_index_format = 0;
			int32_t bytes_per_index = 0;
		} _active_geometry_info;

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
