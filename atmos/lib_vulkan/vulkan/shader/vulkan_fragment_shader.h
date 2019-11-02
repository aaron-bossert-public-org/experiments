
#pragma once

#include <igpu/shader/fragment_shader.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/shader/vulkan_shader.h>
#include <memory>

namespace igpu
{   
    class vulkan_fragment_shader : public fragment_shader, public vulkan_shader
    {
    public:

		struct config : fragment_shader::config
		{
			vulkan vk;
		};

		virtual const config& cfg() const = 0;

		static std::unique_ptr<vulkan_fragment_shader> make(
			const config&);
    };
}
