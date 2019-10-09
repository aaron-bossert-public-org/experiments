
#pragma once

#include <framework/perf/metrics.h>
#include <igpu/resource/vertex_resource.h>
#include <gl/resource/gl_resource.h>
#include <gl/resource/gl_vertex_format.h>

namespace igpu
{
	class gl_vertex_resource : public vertex_resource
	{
	public:

		~gl_vertex_resource() override {};

		const config& cfg() const override;

		void map(buffer_view_base&, size_t count) override;

		void map_subrange(buffer_view_base&, size_t offset, size_t count) override;

		void unmap(buffer_view_base&) override;

		size_t capacity() const override;

		const gl_vertex_format& gl_format() const;

		unsigned gl_handle() const;

		static std::unique_ptr<gl_vertex_resource> make(const config&, const vertex_constraints&);

	protected:

		gl_vertex_resource(const config&, const vertex_constraints&);

	private:

		const config _cfg;
		const gl_vertex_format _format;
		gl_resource _gl_resource;
	};
}
