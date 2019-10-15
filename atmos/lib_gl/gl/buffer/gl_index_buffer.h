
#pragma once

#include <igpu/buffer/index_buffer.h>
#include <gl/buffer/gl_buffer.h>

namespace igpu
{
    class gl_index_buffer : public gl_buffer_t < index_buffer >
    {
    public:

		const config& cfg() const override;

		unsigned gl_format() const;

        static std::unique_ptr<gl_index_buffer> make(const config&);
        
    protected:

        gl_index_buffer(const config&, unsigned gl_format);
        
    private:

		const unsigned _gl_format;
    };
}
