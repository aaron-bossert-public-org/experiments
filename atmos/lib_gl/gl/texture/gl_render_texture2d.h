
#pragma once


#include <framework/perf/metrics.h>
#include <igpu/texture/render_texture2d.h>

namespace igpu
{
    class gl_render_texture2d : public render_texture2d
    {
    public:
        
        unsigned gl_handle() const;
        
        void attach() override;
        
        static std::unique_ptr<gl_render_texture2d> make(const config&);
        
        ~gl_render_texture2d() override;
        
    protected:
        
        gl_render_texture2d(const config&, unsigned gl_handle);
        
    private:
        
        unsigned _gl_handle = 0;
        perf::metric _gpu_mem_metric;
    };
}
