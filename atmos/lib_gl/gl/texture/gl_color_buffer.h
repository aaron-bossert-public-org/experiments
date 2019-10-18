
#pragma once

#include <framework/perf/metrics.h>
#include <igpu/texture/color_target.h>
#include <gl/texture/gl_color_target.h>

namespace igpu
{
    class gl_color_buffer : public color_target, gl_color_target
    {
    public:
        
		const config& cfg() const override;

        void attach() const override;
        
		void gl_handle(int);
		
        static std::unique_ptr<gl_color_buffer> make_empty();
        
        static std::unique_ptr<gl_color_buffer> make(const config&);
        
        ~gl_color_buffer();
        
    protected:
        
        gl_color_buffer(const config&, int gl_handle);
        
    private:
        
		const config _cfg;
        int _gl_handle = 0;
        perf::metric _gpu_mem_metric;
    };
}
