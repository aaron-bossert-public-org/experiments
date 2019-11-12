//#include "gl/context/gl_context.h"
//
//#include "gl/buffer/gl_compute_buffer.h"
//#include "gl/buffer/gl_index_buffer.h"
//#include "gl/buffer/gl_vertex_buffer.h"
//#include "gl/buffer/gl_geometry.h"
//#include "gl/defines/gl_includes.h"
//#include "gl/material/gl_fragment_shader.h"
//#include "gl/material/gl_program.h"
//#include "gl/material/gl_vertex_shader.h"
//#include "gl/material/gl_render_states.h"
//#include "gl/texture/gl_render_buffer.h"
//#include "gl/texture/gl_depth_buffer.h"
//#include "gl/texture/gl_draw_target.h"
//#include "gl/texture/gl_texture2d.h"
//#include "gl/window/gl_back_buffer.h"
//#include "gl/window/gl_window.h"
//
//#include "igpu/batch/batch.h"
//#include "igpu/batch/batch_parameters.h"
//#include "igpu/buffer/topology.h"
//
// using namespace igpu;
//
// std::unique_ptr<gl_context> gl_context::make(
//	const config& cfg,
//	const glm::ivec2& screen_res)
//{
//	glm::ivec2 res = { 800, 600 };
//	auto window = gl_window::make({ cfg.name, }, screen_res);
//
//	if (!window)
//	{
//		LOG_CRITICAL("failed to make window");
//	}
//	else
//	{
//		return std::unique_ptr<gl_context>(
//			new gl_context(
//				cfg,
//				std::move(window)));
//	}
//
//	return nullptr;
//}
//
// const gl_context::config& gl_context::cfg() const
//{
//	return _cfg;
//}
//
// std::unique_ptr<program> gl_context::make_program(
//	const program::config& cfg)
//{
//	ASSERT_CONTEXT(nullptr == active_program());
//
//	gl_program::config gl_cfg = { cfg, this, };
//
//	return gl_program::make(
//		gl_cfg);
//}
//
// std::unique_ptr<vertex_shader> gl_context::make_vertex_shader(
//	const vertex_shader::config& cfg)
//{
//	return gl_vertex_shader::make(cfg);
//}
//
// std::unique_ptr<fragment_shader> gl_context::make_fragment_shader(
//	const fragment_shader::config& cfg)
//{
//	return gl_fragment_shader::make(cfg);
//}
//
// std::unique_ptr<geometry> gl_context::make_geometry(
//	const geometry::config& cfg)
//{
//	return gl_geometry::make(cfg);
//}
//
// std::unique_ptr<vertex_buffer> gl_context::make_vertex_buffer(
//	const vertex_buffer::config& cfg)
//{
//	return gl_vertex_buffer::make(
//		cfg);
//}
//
// std::unique_ptr<index_buffer> gl_context::make_index_buffer(
//	const index_buffer::config& cfg)
//{
//	return gl_index_buffer::make(cfg);
//}
//
// std::unique_ptr<compute_buffer> gl_context::make_compute_buffer(
//	const compute_buffer::config& cfg)
//{
//	return gl_compute_buffer::make(cfg);
//}
//
// std::unique_ptr<texture2d> gl_context::make_texture(
//	const texture2d::config& cfg)
//{
//	return gl_texture2d::make(cfg);
//}
//
// const batch_constraints& gl_context::batch_constraints() const
//{
//	return _batch_constraints;
//}
//
// const material_constraints& gl_context::material_constraints() const
//{
//	return _material_constraints;
//}
//
// const gl_window& gl_context::window() const
//{
//	return *_gl_window;
//}
//
// const gl_back_buffer& gl_context::back_buffer() const
//{
//	return *gl_back_buffer::instance().get();
//}
//
// gl_context::gl_context(
//	const config& cfg,
//	std::unique_ptr<igpu::gl_window> window)
//	: _cfg(cfg)
//	, _batch_constraints(cfg.batch_constraints)
//	, _material_constraints(cfg.material_constraints)
//	, _gl_window(std::move(window))
//	, _packed_states(gl_packed_states({}))
//#if ATMOS_PERFORMANCE_TRACKING
//	, _renderstate_switch_metric(perf::category::SWITCH_RENDER_STATES,
//"Renderstates") 	,
//_draw_target_clears_metric(perf::category::CLEAR_DRAW_TARGET, "Draw Target
// Clears") 	, _draw_target_switch_metric(perf::category::SWITCH_DRAW_TARGET,
//"Draw Target Changes") 	,
//_shader_switch_metric(perf::category::SWITCH_SHADERS, "Shaders") 	,
//_geometry_switch_metric(perf::category::SWITCH_GEOMETRY,
//"geometry Count") 	, _draw_call_metric(perf::category::DRAW_CALL_COUNT,
//"Draw Calls") 	, _polycount_metric(perf::category::POLY_COUNT, "Polycount")
// #endif
//{
//}
//
// gl_context::~gl_context()
//{
//}
//
// void gl_context::set_back_buffer(const gl_back_buffer* back_buffer)
//{
//    ASSERT_CONTEXT(back_buffer);
//
//    if (back_buffer)
//    {
//        glBindFramebuffer(GL_FRAMEBUFFER, back_buffer->gl_handle());
//		back_buffer->color().attach();
//		back_buffer->depth().attach();
//    }
//}
//
// void gl_context::init_render_states()
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	_renderstate_switch_metric.add(1);
//#endif
//
//	set_mode(gl_packed_states::make({}), true);
//
//	_active_texture_stage = GL_TEXTURE0;
//	glActiveTexture(GL_TEXTURE0);
//}
//
// void gl_context::set_render_states(const gl_render_states* render_states,
// bool force)
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	_renderstate_switch_metric.add(1);
//#endif
//
//    set_mode( render_states->packed(), force );
//}
//
// void gl_context::active_texture(GLenum stage, GLuint handle, bool force)
//{
//	if (0 == stage)
//	{
//		LOG_CRITICAL("Attempt to set a non-sampler parameter with texture - it's
// likely a shader didn't compile properly.");
//	}
//	else if (!handle)
//	{
//		LOG_CRITICAL("Attempt to set null texture.");
//	}
//	else if( force || _active_texture_stage != stage )
//	{
//		_active_texture_stage = stage;
//		glActiveTexture( stage );
//		glBindTexture(GL_TEXTURE_2D, handle);
//	}
//}
//
// void gl_context::set_color_write_mask( unsigned color_mask, bool force )
//{
//    if( !force && _packed_states._color_write_mask == color_mask ) return;
//
//	_packed_states._color_write_mask = (uint8_t)color_mask;
//
//	bool maskr = color_mask & 0x08;
//	bool maskg = color_mask & 0x04;
//	bool maskb = color_mask & 0x02;
//	bool maska = color_mask & 0x01;
//
//	glColorMask(maskr, maskg, maskb, maska);
//}
//
//// provided macro for testing if states have changed, and updating existing
/// value
//                                #define test_redundancy( mode, force ) \
//if( !force && !memcmp( _packed_states._gl_##mode, mode ) ) return;\
//_packed_states._gl_##mode = mode;
//
// void gl_context::set_mode( const gl_cull_mode& cull_mode, bool force )
//{
//	test_redundancy(cull_mode, force);
//    if (cull_mode.enable)
//    {
//        glEnable(GL_CULL_FACE);
//        glCullFace(cull_mode.mode);
//    }
//    else
//    {
//        glDisable(GL_CULL_FACE);
//    }
//}
//
// void gl_context::set_mode( const gl_blend_mode& blend_mode, bool force )
//{
//	test_redundancy(blend_mode, force);
//
//    if (blend_mode.enable)
//    {
//        glEnable(GL_BLEND);
//        glBlendFunc(blend_mode.src, blend_mode.dst);
//    }
//    else
//    {
//        glDisable(GL_BLEND);
//    }
//}
//
// void gl_context::set_mode( const gl_depth_mode& depth_mode, bool force )
//{
//	test_redundancy(depth_mode, force);
//
//    glDepthMask(depth_mode.write);
//    if (depth_mode.enable)
//    {
//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(depth_mode.compare);
//    }
//    else
//    {
//        glDisable(GL_DEPTH_TEST);
//    }
//
//}
//
// void gl_context::set_mode( const gl_stencil_mode& stencil_mode, bool force )
//{
//	test_redundancy(stencil_mode, force);
//
//    glStencilMask(stencil_mode.write_mask);
//    if (stencil_mode.enable)
//    {
//        glEnable(GL_STENCIL_TEST);
//        glStencilFunc(stencil_mode.compare, stencil_mode.ref,
//        stencil_mode.read_mask); glStencilOp(stencil_mode.fail,
//        stencil_mode.depth_fail, stencil_mode.pass);
//    }
//    else
//    {
//        glDisable(GL_STENCIL_TEST);
//    }
//}
//
// void gl_context::set_mode( const gl_packed_states& packed, bool force )
//{
//    set_color_write_mask(packed._color_write_mask, force );
//    set_mode(packed._gl_cull_mode, force );
//    set_mode(packed._gl_blend_mode, force );
//    set_mode(packed._gl_depth_mode, force );
//    set_mode(packed._gl_stencil_mode, force );
//}
//
// const gl_vertex_attrib_array_tracker&
// gl_context::vertex_attrib_array_tracker() const
//{
//    return _vertex_attrib_array_tracker;
//}
//
// void gl_context::begin_batch(batch* b)
//{
//	ASSERT_CONTEXT(b);
//	ASSERT_CONTEXT(_active_batch == nullptr);
//    init_render_states();
//    _vertex_attrib_array_tracker.state_caching_enabled(true);
//	_active_batch = b;
//	++_batch_drawpass_id;
//}
//
// void gl_context::end_batch(const batch* b)
//{
//	ASSERT_CONTEXT(b);
//	ASSERT_CONTEXT(_active_batch == b);
//	_active_batch = nullptr;
//    _vertex_attrib_array_tracker.state_caching_enabled(false);
//	++_batch_drawpass_id;
//}
//
// const batch* gl_context::active_batch() const
//{
//	return _active_batch;
//}
//
// void gl_context::set_viewport()
//{
//    GLint render_buffer = 0;
//    glGetIntegerv(GL_RENDERBUFFER_BINDING, &render_buffer);
//
//    if(render_buffer)
//    {
//		utility::viewport viewport;
//
//        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH,
//        &viewport.w); glGetRenderbufferParameteriv(GL_RENDERBUFFER,
//        GL_RENDERBUFFER_HEIGHT, &viewport.h);
//
//        set_viewport(viewport);
//    }
//}
//
// void gl_context::set_viewport(const utility::viewport& viewport)
//{
//    glViewport(viewport.x, viewport.y, viewport.w, viewport.h);
//    _current_viewport = viewport;
//}
//
// void gl_context::set_scissor()
//{
//    /// only need to turn it off...
//    glDisable(GL_SCISSOR_TEST);
//
//    glDepthRangef(0.0f, 1.0f);
//}
//
// const utility::viewport& gl_context::get_viewport() const
//{
//	return _current_viewport;
//}
//
// void gl_context::set_scissor(const utility::scissor& scissor)
//{
//    glScissor(scissor.x, scissor.y, scissor.w, scissor.h);
//    glEnable(GL_SCISSOR_TEST);
//
//    glDepthRangef(scissor.zmin, scissor.zmax);
//}
//
// void gl_context::clear(const glm::vec4* color, const float* depth, const
// uint8_t* stencil)
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	_draw_target_clears_metric.add(1);
//#endif
//
//    GLbitfield mask = 0;
//
//    if(color)
//    {
//        mask |= GL_render_BUFFER_BIT;
//
//    	glClearColor(color->r, color->g, color->b, color->a);
//    }
//
//    if(depth)
//    {
//        mask |= GL_DEPTH_BUFFER_BIT;
//    	glDepthMask(true);
//    	glClearDepthf(*depth);
//    }
//
//    if(stencil)
//    {
//        mask |= GL_STENCIL_BUFFER_BIT;
//        glStencilMask(0xff);
//        glClearStencil(*stencil);
//    }
//
//    glClear(mask);
//}
//
// void gl_context::set_draw_target(gl_draw_target* target)
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	_draw_target_switch_metric.add(1);
//#endif
//
//    ASSERT_CONTEXT(target, "gl_context::setDrawTarget(target) failed, target
//    is null"); if (nullptr != target)
//    {
//        glBindFramebuffer(GL_FRAMEBUFFER, target->frame_buffer());
//    }
//}
//
// void gl_context::begin_program(gl_program* program)
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	_shader_switch_metric.add(1);
//#endif
//
//	ASSERT_CONTEXT(nullptr != program);
//	ASSERT_CONTEXT(nullptr == _active_program);
//
//    _active_program = program;
//    glUseProgram(_active_program->gl_handle());
//	_active_program->update(_batch_drawpass_id, _active_batch);
//}
//
// void gl_context::end_program(const gl_program* program)
//{
//	ASSERT_CONTEXT(nullptr != program);
//    ASSERT_CONTEXT(_active_program == program);
//    _active_program = nullptr;
//}
//
// const gl_program* gl_context::active_program() const
//{
//	return _active_program;
//}
//
//
// void gl_context::begin_geometry(const gl_geometry* geometry)
//{
//#if ATMOS_PERFORMANCE_TRACKING
//	_geometry_switch_metric.add(1);
//#endif
//
//	ASSERT_CONTEXT(geometry);
//	ASSERT_CONTEXT(_active_program);
//	ASSERT_CONTEXT(!_active_geometry_info.geometry);
//
//	auto* gl_indices = ASSERT_CAST(gl_index_buffer*,
// geometry->cfg().index_buffer.get());
//
//	auto vao = geometry->vao();
//	glBindVertexArray(vao);
//
//	_active_geometry_info = {};
//	_active_geometry_info.geometry = geometry;
//	_active_geometry_info.gl_topology = geometry->gl_topology();
//	_active_geometry_info.gl_index_format = gl_indices->gl_format();
//	_active_geometry_info.bytes_per_index =
//(int32_t)bytes_per_index(gl_indices->cfg().format);
//}
//
// void gl_context::end_geometry(const gl_geometry* geometry)
//{
//	ASSERT_CONTEXT(geometry);
//	ASSERT_CONTEXT(_active_geometry_info.geometry == geometry);
//
//	glBindVertexArray(0);
//	_active_geometry_info = {};
//}
//
// void gl_context::draw(int32_t start, int32_t count)
//{
//	ASSERT_CONTEXT(count);
//	ASSERT_CONTEXT(_active_batch);
//	ASSERT_CONTEXT(_active_program);
//	ASSERT_CONTEXT(_active_geometry_info.geometry);
//
//	if (!count || !_active_batch || !_active_program ||
//!_active_geometry_info.geometry)
//	{
//		return;
//	}
//
//#if ATMOS_PERFORMANCE_TRACKING
//	_polycount_metric.add(polycount(_active_geometry->topology(), count));
//#endif
//
//#if ATMOS_DEBUG
//	size_t index_count =
//_active_geometry_info.geometry->cfg().index_buffer->byte_capacity() /
//_active_geometry_info.bytes_per_index; 	ASSERT_CONTEXT(start <=
// index_count); 	ASSERT_CONTEXT(start + count <= index_count); #endif
//
//	glDrawElements(
//		_active_geometry_info.gl_topology,
//		count,
//		_active_geometry_info.gl_index_format,
//		(start * _active_geometry_info.bytes_per_index) + (char*)nullptr);
//}
//
// void gl_context::flush()
//{
//    glFlush();
//}
