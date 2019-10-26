
#pragma once

#include <igpu/buffer/compute_buffer.h>
#include <igpu/buffer/geometry.h>
#include <igpu/buffer/index_buffer.h>
#include <igpu/buffer/topology.h>
#include <igpu/buffer/vertex_buffer.h>
#include <igpu/context/batch_constraints.h>
#include <igpu/context/material_constraints.h>
#include <igpu/context/vertex_constraints.h>
#include <igpu/material/fragment_shader.h>
#include <igpu/material/program.h>
#include <igpu/material/vertex_shader.h>
#include <igpu/texture/color_format.h>
#include <igpu/texture/depth_format.h>
#include <igpu/texture/texture2d.h>
#include <functional>
#include <memory>

namespace igpu
{
	class back_buffer;

	class batch_parameters;
	class window;

	class context
	{
	public:

		struct config
		{
			std::string name;
			batch_constraints::config batch_constraints;
			material_constraints::config material_constraints;
			vertex_constraints::config vertex_constraints;
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

		virtual std::unique_ptr<program> make_program(
			const program::config&) = 0;

		virtual std::unique_ptr<vertex_shader> make_vertex_shader(
			const vertex_shader::config&) = 0;
		
		virtual std::unique_ptr<fragment_shader> make_fragment_shader(
			const fragment_shader::config&) = 0;

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

		virtual const batch_constraints& batch_constraints() const = 0;

		virtual const material_constraints& material_constraints() const = 0;

		virtual const vertex_constraints& vertex_constraints() const = 0;

		virtual const window& window() const = 0;

		virtual const back_buffer& back_buffer() const = 0;

		virtual ~context() = default;

	protected:

		context() = default;
		context(const context&) = delete;
		context& operator= (const context&) = delete;
	};
}
