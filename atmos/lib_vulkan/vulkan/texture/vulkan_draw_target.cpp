
// #include <vulkan/texture/vulkan_draw_target.h>

// // Vulkan implementation includes - begin
// #include <vulkan/defines/vulkan_includes.h>
// // Vulkan implementation includes - end

// #include <igpu/texture/color_target.h>
// #include <igpu/texture/depth_target.h>

// using namespace igpu;

// unsigned int vulkan_draw_target::frame_buffer() const
// {
//     return _frame_buffer;
// }

// bool vulkan_draw_target::validate_framebuffer_status(int status)
// {
//     if (status != GL_FRAMEBUFFER_COMPLETE)
//     {
// 		std::string err = 
// #ifdef GL_FRAMEBUFFER_UNDEFINED
// 			(status == GL_FRAMEBUFFER_UNDEFINED) ?
// 			"GL_FRAMEBUFFER_UNDEFINED: \n"
// 			"Target is the default framebuffer, and the default framebuffer does not exist." :
// #endif

// #ifdef GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT
// 			(status == GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) ?
// 			"GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: \n"
// 			"A framebuffer attachment point is framebuffer incomplete." :
// #endif

// #ifdef GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT
// 			(status == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) ?
// 			"GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: \n"
// 			"The framebuffer does not have at least one image attached to it." :
// #endif

// #ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
// 			(status == GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) ?
// 			"GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: \n"
// 			"The value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for a color attachment point named by GL_DRAW_BUFFERi" :
// #endif

// #ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
// 			(status == GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) ?
// 			"GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: \n"
// 			"GL_READ_BUFFER is not GL_NONE and the value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for the color attachment point named by GL_READ_BUFFER." :
// #endif

// #ifdef GL_FRAMEBUFFER_UNSUPPORTED
// 			(status == GL_FRAMEBUFFER_UNSUPPORTED) ?
// 			"GL_FRAMEBUFFER_UNSUPPORTED: \n"
// 			"The combination of internal formats of the attached images violates an implementation-dependent set of restrictions." :
// #endif

// #ifdef GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE
// 			(status == GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE) ?
// 			"GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: One of or more of the following has happened...\n"
// 			"1. The value of GL_RENDERBUFFER_SAMPLES is not the same for all attached renderbuffers \n"
// 			"2. The value of GL_TEXTURE_SAMPLES is the not same for all attached textures or, if the attached images are a mix of renderbuffers and textures, the value of GL_RENDERBUFFER_SAMPLES does not match the value of GL_TEXTURE_SAMPLES. \n"
// 			"3. The value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for all attached textures \n"
// 			"4. The attached images are a mix of renderbuffers and textures, the value of GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not GL_TRUE for all attached textures." :
// #endif

// #ifdef GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS
// 			(status == GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS) ?
// 			"GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: \n"
// 			"A framebuffer attachment is layered, and a populated attachment is not layered, or a populated color attachment is not from texture of the same target." :
// #endif

// 			"unknown error!";

// 		LOG_CONTEXT( CRITICAL, "glCheckFramebufferStatus(GL_FRAMEBUFFER) returned %d\n%s",
// 			status, err.c_str());

//         return false;
//     }
    
//     return true;
// }

// std::unique_ptr<vulkan_draw_target> vulkan_draw_target::make(
// 	const std::shared_ptr<color_target>& color_target,
//	const std::shared_ptr<depth_target>& depth_target)
// {
//     GLuint frame_buffer = 0;
    
//     if(!color_target)
//     {
//         LOG_CONTEXT( CRITICAL, "color_target null");
//     }
//     if(!depth_target)
//     {
//         LOG_CONTEXT( CRITICAL, "depth_target null");
//     }
//     else
//     {
//         GLint active_frame_buffer = 0;
        
//         glGetIntegerv(GL_FRAMEBUFFER_BINDING, &active_frame_buffer);
        
//         glGenFramebuffers(1, &frame_buffer);
        
//         glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
        
//         depth_target->attach();
//         color_target->attach();
        
//         GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        
//         glBindFramebuffer(GL_FRAMEBUFFER, active_frame_buffer);
        
//         if(validate_framebuffer_status(status))
//         {
//             return std::unique_ptr<vulkan_draw_target>(new vulkan_draw_target(frame_buffer, color_target, depth_target));
//         }
//     }
    
//     glDeleteFramebuffers(1, &frame_buffer);
    
//     return nullptr;
// }

// vulkan_draw_target::~vulkan_draw_target()
// {
//     glDeleteFramebuffers(1, &_frame_buffer);
// }

// vulkan_draw_target::vulkan_draw_target(unsigned int frame_buffer,
//                            const std::shared_ptr<color_target>& color_target,
//                            const std::shared_ptr<depth_target>& depth_target)
// : draw_target(color_target, depth_target)
// , _frame_buffer(frame_buffer)
// {
// }
