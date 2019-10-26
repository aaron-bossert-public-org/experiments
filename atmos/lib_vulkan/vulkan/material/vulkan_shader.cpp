
#include <vulkan/material/vulkan_vertex_shader.h>
#include <vulkan/material/vulkan_shader.h>
#include <vulkan/material/vulkan_compiler.h>

using namespace igpu;

vulkan_shader::vulkan_shader(const vulkan_shader::config& cfg)
: _cfg(cfg)
, _buffer({ cfg.usage })
{
}

vulkan_shader::~vulkan_shader()
{
	if (_shader_module)
	{
		vkDestroyShaderModule(_cfg.device, _shader_module, nullptr);
	}
}

const vulkan_shader::config& vulkan_shader::cfg() const
{
	return _cfg;
}

VkShaderModule vulkan_shader::shader_module() const
{
	return _shader_module;
}

void vulkan_shader::map(
	size_t byte_size, 
	buffer_view_base* out_buffer_view)
{
	_buffer.map(byte_size, out_buffer_view);
}

void vulkan_shader::unmap()
{
	auto mapped_view = _buffer.mapped_view();
	if (mapped_view.data())
	{
		if (_shader_module)
		{
			vkDestroyShaderModule(_cfg.device, _shader_module, nullptr);
		}

		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = mapped_view.byte_size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(mapped_view.data());

		vkCreateShaderModule(_cfg.device, &create_info, nullptr, &_shader_module);
	}

	_buffer.unmap();
}

size_t vulkan_shader::byte_size() const
{
	return _buffer.byte_size();
}
