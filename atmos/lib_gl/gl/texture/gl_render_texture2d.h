
#pragma once


#include <framework/perf/metrics.h>
#include <igpu/texture/render_texture2d.h>
#include <gl/texture/gl_color_target.h>

namespace igpu
{
    class gl_render_texture2d : public render_texture2d, gl_color_target
    {
    public:

		const config& cfg() const override;
        
		void attach() const override;
		
		unsigned gl_handle() const;

        static std::unique_ptr<gl_render_texture2d> make(const config&);
        
        ~gl_render_texture2d() override;
        
    protected:
        
        gl_render_texture2d(const config&, unsigned gl_handle);
        
    private:
        
		const config _cfg;
        unsigned _gl_handle = 0;
        perf::metric _gpu_mem_metric;
    };
}
