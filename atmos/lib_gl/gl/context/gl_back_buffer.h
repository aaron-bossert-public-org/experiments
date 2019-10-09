
#pragma once

#include <igpu/context/back_buffer.h>

namespace igpu
{
    class gl_depth_buffer;
    class gl_color_buffer;
    
    class gl_back_buffer : public back_buffer
    {
    public:
        
		const config& cfg() const override;

        int gl_handle() const;
        
        void gl_handle(int);
        
        static std::shared_ptr<gl_back_buffer> instance();
        
    private:
        
        static std::unique_ptr<gl_back_buffer> make(const config&);
        
        gl_back_buffer(const config&);
        
    private:
        
		const config _cfg;
        int _gl_handle = 0;
    };
}
