
#pragma once


#include <igpu/texture/texture2d.h>
#include <framework/perf/metrics.h>
#include <glm/vec2.hpp>

namespace igpu
{
    class gl_texture2d : public texture2d
    {
    public:

		const config& cfg() const override;

        unsigned gl_handle() const;
        
        static std::unique_ptr<gl_texture2d> make(const texel_buffer2d&);
        
        ~gl_texture2d() override;

        gl_texture2d(
			const config&,
			size_t size,
			unsigned gl_handle);
        
    private:
        
		const config _cfg;
        unsigned _gl_handle;
        
        perf::metric _gpu_mem_metric;
    };
}
