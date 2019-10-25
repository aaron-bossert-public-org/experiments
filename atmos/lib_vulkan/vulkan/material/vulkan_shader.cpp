
#include <vulkan/material/vulkan_vertex_shader.h>
#include <vulkan/material/vulkan_shader.h>
#include <vulkan/material/vulkan_compiler.h>

using namespace igpu;

vulkan_shader::vulkan_shader(const vulkan_shader::config& cfg)
: _cfg(cfg)
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
	if (_mapped.raw)
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else if(_shader_module)
	{
		LOG_CRITICAL("cannot map shader multiple times");
	}
	else
	{
		_mapped.raw.reset(new char[byte_size]);
		_mapped.view = 
			*out_buffer_view = buffer_view_base(
			byte_size,
			_mapped.raw.get(),
			out_buffer_view->stride());
	}
}

void vulkan_shader::unmap()
{
	if (!_mapped.raw)
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else if (_shader_module)
	{
		LOG_CRITICAL("cannot map shader multiple times");
	}
	else
	{
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = _mapped.view.byte_size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(_mapped.view.data());

		vkCreateShaderModule(_cfg.device, &create_info, nullptr, &_shader_module);

		_mapped.raw.reset();
		_mapped.view.reset();
	}
}
