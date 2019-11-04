
#pragma once

#include "igpu/texture/draw_target.h"
#include <memory>

namespace igpu
{
    class gl_draw_target : public draw_target
    {
    public:

        unsigned int frame_buffer() const;
        
        static bool validate_framebuffer_status(int status);
        
        static std::unique_ptr<gl_draw_target> make(
            const std::shared_ptr<render_target>&,
            const std::shared_ptr<depth_target>&);
        
        ~gl_draw_target() override;
        
    private:
        gl_draw_target(
			unsigned int frame_buffer,
			const std::shared_ptr<render_target>&,
			const std::shared_ptr<depth_target>&);
        
        unsigned int _frame_buffer;
    };
}
