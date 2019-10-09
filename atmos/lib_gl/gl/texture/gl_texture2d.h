
#pragma once


#include <igpu/texture/texture2d.h>
#include <framework/perf/metrics.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class gl_texture2d : public texture2d
    {
    public:
        
        unsigned gl_handle() const;
        
    public:
        
        static std::unique_ptr<gl_texture2d> make(const texel_buffer2d&);
        
        virtual ~gl_texture2d();

        gl_texture2d(
			const texel_buffer2d::config&,
			size_t size,
			unsigned gl_handle);
        
    private:
        
        unsigned _gl_handle;
        
        perf::metric _gpu_mem_metric;
    };
}
