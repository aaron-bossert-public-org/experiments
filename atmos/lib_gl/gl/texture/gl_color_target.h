
#pragma once

namespace igpu
{
    class gl_color_target
    {
    public:

		virtual void attach () const = 0;
        
		virtual ~gl_color_target() {}
        
    protected:
       
		gl_color_target() = default;
		gl_color_target(const gl_color_target&) = delete;
		gl_color_target& operator= (const gl_color_target&) = delete;
    };
}
