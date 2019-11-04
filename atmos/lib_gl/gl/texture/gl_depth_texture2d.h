
#pragma once

#include "igpu/texture/depth_texture2d.h"
#include "framework/perf/metrics.h"
#include "gl/texture/gl_depth_target.h"

namespace igpu
{
    class gl_depth_texture2d : public depth_texture2d, gl_depth_target
    {
    public:

		const config& cfg() const override;

        void attach() const override;
        
        unsigned gl_handle() const;
        
        static std::unique_ptr<gl_depth_texture2d> make(const config&);
        
        ~gl_depth_texture2d();
        
    protected:
        
        gl_depth_texture2d(const config&, unsigned gl_handle);
        
    private:
        
		const config _cfg;
        unsigned _gl_handle = 0;
        perf::metric _gpu_mem_metric;
    };
}
