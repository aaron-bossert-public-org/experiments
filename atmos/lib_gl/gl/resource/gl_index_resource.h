
#pragma once

#include <igpu/resource/index_resource.h>
#include <gl/resource/gl_resource.h>

namespace igpu
{
    class gl_index_resource : public index_resource
    {
    public:

		~gl_index_resource() override {};

		const config& cfg() const override;

		void map(buffer_view_base&, size_t count) override;

		void map_subrange(buffer_view_base&, size_t offset, size_t count) override;

		void unmap(buffer_view_base&) override;

		size_t capacity() const override;

		unsigned gl_format() const;

		unsigned gl_handle() const;

        static std::unique_ptr<gl_index_resource> make(const config&);
        
    protected:

        gl_index_resource(const config&, unsigned gl_format);
        
    private:

		const config _cfg;
		const unsigned _gl_format;
		gl_resource _gl_resource;
    };
}
