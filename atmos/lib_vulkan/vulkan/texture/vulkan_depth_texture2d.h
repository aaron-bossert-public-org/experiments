
// #pragma once

// #include <igpu/texture/depth_texture2d.h>
// #include <framework/perf/metrics.h>
        
// namespace igpu
// {
//     class vulkan_depth_texture2d : public depth_texture2d
//     {
//     public:
        
//         void attach() override;
        
//         unsigned gl_handle() const;
        
//         static std::unique_ptr<vulkan_depth_texture2d> make(const config&);
        
//         ~vulkan_depth_texture2d();
        
//     protected:
        
//         vulkan_depth_texture2d(const config&, unsigned gl_handle);
        
//     private:
        
//         unsigned _gl_handle = 0;
//         perf::metric _gpu_mem_metric;
//     };
// }
