
#pragma once

#include <igpu/batch/opaque_batch.h>
#include <igpu/batch/transparent_batch.h>
#include <igpu/buffer/compute_buffer.h>
#include <igpu/buffer/geometry.h>
#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/topology.h>
#include <igpu/buffer/vertex_buffer.h>
#include <igpu/context/batch_constraints.h>
#include <igpu/context/material_constraints.h>
#include <igpu/shader/fragment_shader.h>
#include <igpu/shader/program.h>
#include <igpu/shader/render_states.h>
#include <igpu/shader/vertex_shader.h>
#include <igpu/texture/color_format.h>
#include <igpu/texture/draw_target.h>
#include <igpu/texture/depth_buffer.h>
#include <igpu/texture/depth_texture2d.h>
#include <igpu/texture/depth_format.h>
#include <igpu/texture/render_buffer.h>
#include <igpu/texture/render_texture2d.h>
#include <igpu/texture/texture2d.h>
#include <functional>
#include <memory>

namespace igpu
{
	class back_buffer;
	class window;

	class context
	{
	public:

		struct config
		{
			std::string name;
			batch_constraints::config batch_constraints;
			material_constraints::config material_constraints;
			color_format color_format;
			depth_format depth_format;

			bool enable_validation 
#if ATMOS_DEBUG
				= true;
#else 
				= false;
#endif
		};

		virtual const config& cfg() const = 0;

		virtual std::unique_ptr<draw_target> make_draw_target(
			const draw_target::config&) = 0;

		virtual std::unique_ptr<render_buffer> make_render_buffer(
			const render_buffer::config&) = 0;

		virtual std::unique_ptr<render_texture2d> make_render_texture2d(
			const render_texture2d::config&) = 0;

		virtual std::unique_ptr<depth_buffer> make_depth_buffer(
			const depth_buffer::config&) = 0;

		virtual std::unique_ptr<depth_texture2d> make_depth_texture2d(
			const depth_texture2d::config&) = 0;

		virtual std::unique_ptr<program> make_program(
			const program::config&) = 0;

		virtual std::unique_ptr<vertex_shader> make_vertex_shader(
			const vertex_shader::config&) = 0;
		
		virtual std::unique_ptr<fragment_shader> make_fragment_shader(
			const fragment_shader::config&) = 0;

		virtual std::unique_ptr<render_states> make_render_states(
			const render_states::config&) = 0;

		virtual std::unique_ptr<geometry> make_geometry(
			const geometry::config&) = 0;

		virtual std::unique_ptr<vertex_buffer> make_vertex_buffer(
			const vertex_buffer::config&) = 0;

		virtual std::unique_ptr<index_buffer> make_index_buffer(
			const index_buffer::config&) = 0;

		virtual std::unique_ptr<compute_buffer> make_compute_buffer(
			const compute_buffer::config&) = 0;

		virtual std::unique_ptr<texture2d> make_texture(
			const texture2d::config&) = 0;

		virtual std::unique_ptr<opaque_batch> make_opaque_batch(
			const opaque_batch::config&) = 0;

		virtual std::unique_ptr<transparent_batch> make_transparent_batch(
			const transparent_batch::config&) = 0;

		virtual const batch_constraints& batch_constraints() const = 0;

		virtual const material_constraints& material_constraints() const = 0;

		virtual const window& window() const = 0;

		virtual const back_buffer& back_buffer() const = 0;

		virtual ~context() = default;

	protected:

		context() = default;
		context(const context&) = delete;
		context& operator= (const context&) = delete;
	};
}
