
// #pragma once

// #include <igpu/texture/draw_target.h>

// #include <memory>

// namespace igpu
// {
//     class vulkan_draw_target : public draw_target
//     {
//     public:
        
//         unsigned int frame_buffer() const;
        
//         static bool validate_framebuffer_status(int status);
        
//         static std::unique_ptr<vulkan_draw_target> make(const std::shared_ptr<color_target>&,
//                                                     const std::shared_ptr<depth_target>&);
        
//         virtual ~vulkan_draw_target();
        
//     private:
//         vulkan_draw_target(unsigned int frame_buffer,
//                      const std::shared_ptr<color_target>&,
//                      const std::shared_ptr<depth_target>&);
        
//         unsigned int _frame_buffer;
//     };
// }
