
#pragma once

#include <igpu/window/back_buffer.h>

#include <gl/texture/gl_color_buffer.h>
#include <gl/texture/gl_depth_buffer.h>

#include <memory>

namespace igpu
{
    class gl_color_buffer;
    class gl_depth_buffer;
    
    class gl_back_buffer : public back_buffer
    {
    public:
        
		const config& cfg() const override;

		const gl_color_buffer& color() const override;

		const gl_depth_buffer& depth() const override;

        int gl_handle() const;
        
        void gl_handle(int);
        
        static std::shared_ptr<gl_back_buffer> instance();
        
    private:

        gl_back_buffer(
			const config&,
			const std::shared_ptr <gl_color_buffer>&,
			const std::shared_ptr <gl_depth_buffer>&);
        
    private:
        
		const config _cfg;
		std::shared_ptr <gl_color_buffer> _color;
		std::shared_ptr <gl_depth_buffer> _depth;
        int _gl_handle = 0;
    };
}
