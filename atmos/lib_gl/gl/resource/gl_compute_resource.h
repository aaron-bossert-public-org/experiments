
#pragma once

#include <igpu/resource/compute_resource.h>
#include <gl/resource/gl_resource.h>

namespace igpu
{
	class gl_compute_resource : public compute_resource
	{
	public:

		~gl_compute_resource() override {};
		
		const config& cfg() const override;

		void map(buffer_view_base&, size_t count) override;

		void map_subrange(buffer_view_base&, size_t offset, size_t count) override;

		void unmap(buffer_view_base&) override;

		size_t capacity() const override;
		
		unsigned gl_handle() const;

		static std::unique_ptr<gl_compute_resource> make(const config&);

	protected:

		gl_compute_resource(const config&);

	private:

		const config _cfg;
		gl_resource _gl_resource;
	};
}