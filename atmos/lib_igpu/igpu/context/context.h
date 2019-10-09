
#pragma once

#include <igpu/context/batch_constraints.h>
#include <igpu/context/material_constraints.h>
#include <igpu/context/vertex_constraints.h>
#include <igpu/resource/compute_resource.h>
#include <igpu/resource/index_resource.h>
#include <igpu/resource/topology.h>
#include <igpu/resource/vertex_resource.h>
#include <functional>
#include <memory>

namespace igpu
{
	class batch_parameters;
	class geometry;
	class program;
	class window;

	class context
	{
	public:

		struct config
		{
			batch_constraints::config batch_constraints;
			material_constraints::config material_constraints;
			vertex_constraints::config vertex_constraints;
		};

		virtual std::unique_ptr<program> make_program(
			const buffer_view<uint8_t>& vertex_code,
			const buffer_view<uint8_t>& pixel_code) = 0;

		virtual std::unique_ptr<geometry> make_geometry(
			std::string name,
			topology,
			size_t element_start,
			size_t element_count,
			std::vector<std::shared_ptr<vertex_resource>>,
			std::shared_ptr<index_resource>) = 0;

		virtual std::unique_ptr<vertex_resource> make_vertex_resource(
			const vertex_resource::config&) = 0;

		virtual std::unique_ptr<index_resource> make_index_resource(
			const index_resource::config&) = 0;

		virtual std::unique_ptr<compute_resource> make_compute_resource(
			const compute_resource::config&) = 0;
		
		virtual ~context() = 0;

		virtual const batch_constraints& batch_constraints() const = 0;

		virtual const material_constraints& material_constraints() const = 0;

		virtual const vertex_constraints& vertex_constraints() const = 0;

		virtual const window& window() const = 0;

	protected:

		context() = default;
		context(const context&) = delete;
		context& operator= (const context&) = delete;
	};
}
