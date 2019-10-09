
// #pragma once


// #include <framework/perf/metrics.h>
// #include <igpu/texture/render_texture2d.h>

// namespace igpu
// {
//     class vulkan_render_texture2d : public render_texture2d
//     {
//     public:
        
//         unsigned gl_handle() const;
        
//         void attach() override;
        
//         static std::unique_ptr<vulkan_render_texture2d> make(const config&);
        
//         ~vulkan_render_texture2d() override;
        
//     protected:
        
//         vulkan_render_texture2d(const config&, unsigned gl_handle);
        
//     private:
        
//         unsigned _gl_handle = 0;
//         perf::metric _gpu_mem_metric;
//     };
// }
