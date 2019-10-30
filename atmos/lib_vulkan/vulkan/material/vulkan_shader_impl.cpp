
#include <vulkan/material/vulkan_vertex_shader.h>
#include <vulkan/material/vulkan_shader_impl.h>
#include <vulkan/material/vulkan_compiler.h>

#include <igpu/material/program_parsing.h>

using namespace igpu;

vulkan_shader_impl::vulkan_shader_impl(const vulkan_shader_impl::config& cfg)
: _cfg(cfg)
, _cpu_mem_metric(perf::category::CPU_MEM_USAGE, "spirv buffer")
{
}

vulkan_shader_impl::~vulkan_shader_impl()
{
	if (_shader_module)
	{
		vkDestroyShaderModule(_cfg.vk.device, _shader_module, nullptr);
	}
}

const vulkan_shader_impl::config& vulkan_shader_impl::cfg() const
{
	return _cfg;
}

VkShaderModule vulkan_shader_impl::shader_module() const
{
	return _shader_module;
}

void vulkan_shader_impl::map(
	size_t byte_size, 
	buffer_view_base* out_mapped_view)
{
	if (_memory.size())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		
		_memory.resize((byte_size + _element_size - 1) / _element_size);

		_cpu_mem_metric.reset();
		_cpu_mem_metric.add(_memory.size() * sizeof(_memory[0]));

		size_t stride = out_mapped_view->stride();
		*out_mapped_view = buffer_view_base(
			byte_size / stride,
			_memory.data(),
			stride);
	}
}

void vulkan_shader_impl::unmap()
{
	if (!_memory.size())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		if (_shader_module)
		{
			vkDestroyShaderModule(_cfg.vk.device, _shader_module, nullptr);
		}

		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = _memory.size() * _element_size;
		create_info.pCode = _memory.data();

		vkCreateShaderModule(_cfg.vk.device, &create_info, nullptr, &_shader_module);
		
		_resources.clear();
		_attributes.clear();

		auto* p_attributes = &_attributes;
		if (_cfg.vk.shader_stage  != VK_SHADER_STAGE_VERTEX_BIT)
		{
			p_attributes = nullptr;
		}

		parse_spirv(std::move(_memory), &_resources, p_attributes);
	}
}

size_t vulkan_shader_impl::byte_capacity() const
{
	return _memory.capacity() * sizeof(_memory[0]);
}

size_t vulkan_shader_impl::resource_count() const
{
	return _resources.size();
}

const spirv_resource& vulkan_shader_impl::resource(size_t i) const
{
	return _resources[i];
}

size_t vulkan_shader_impl::attribute_count() const
{
	return _attributes.size();
}

const spirv_attribute& vulkan_shader_impl::attribute(size_t i) const
{
	return _attributes[i];
}
