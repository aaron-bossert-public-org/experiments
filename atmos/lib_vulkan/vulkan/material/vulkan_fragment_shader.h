
#pragma once

#include <igpu/material/fragment_shader.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/material/vulkan_shader.h>
#include <memory>

namespace igpu
{   
    class vulkan_fragment_shader : public fragment_shader, public vulkan_shader
    {
    public:
		
        static std::unique_ptr<vulkan_fragment_shader> make(
			const config&,
			VkDevice);
    };
}
