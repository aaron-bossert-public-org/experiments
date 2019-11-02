
#include <vulkan/shader/vulkan_vertex_shader.h>
#include <vulkan/shader/vulkan_shader_impl.h>
#include <vulkan/shader/vulkan_compiler.h>
#include <vulkan/shader/vulkan_shader_stages.h>

#include <igpu/shader/program_parsing.h>

using namespace igpu;

namespace
{
	const std::string s_entry_point = "main";
}

vulkan_shader_impl::vulkan_shader_impl(const vulkan_shader_impl::config& cfg)
: _cfg(cfg)
, _cpu_mem_metric(perf::category::CPU_MEM_USAGE, "spirv buffer")
{
}

vulkan_shader_impl::~vulkan_shader_impl()
{
	release();
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
		release();

		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = _memory.size() * _element_size;
		create_info.pCode = _memory.data();

		vkCreateShaderModule(_cfg.vk.device, &create_info, nullptr, &_shader_module);
		
		_parameters.clear();
		_vertex_parameters.clear();

		shader_stages stages = from_vulkan_stage_flags(_cfg.vk.stage_flags);
		
		spirv::parse(std::move(_memory), s_entry_point, stages, &_parameters, &_vertex_parameters);
	}
}

size_t vulkan_shader_impl::byte_capacity() const
{
	return _memory.capacity() * sizeof(_memory[0]);
}


VkPipelineShaderStageCreateInfo vulkan_shader_impl::stage_info() const
{
	VkPipelineShaderStageCreateInfo shader_stage_info = {};
	shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_info.stage = _cfg.vk.stage_flags;
	shader_stage_info.module = _shader_module;
	shader_stage_info.pName = s_entry_point.c_str();
	return shader_stage_info;
}

size_t vulkan_shader_impl::parameter_count() const
{
	return _parameters.size();
}

const spirv::parameter& vulkan_shader_impl::parameter(size_t i) const
{
	return _parameters[i];
}

size_t vulkan_shader_impl::vertex_parameter_count() const
{
	return _vertex_parameters.size();
}

const spirv::vertex_parameter& vulkan_shader_impl::vertex_parameter(size_t i) const
{
	return _vertex_parameters[i];
}

void vulkan_shader_impl::release()
{
	if (_shader_module)
	{
		// it is safe to destroy shader modules while they are in use
		vkDestroyShaderModule(_cfg.vk.device, _shader_module, nullptr);
	}

	_parameters.clear();
	_vertex_parameters.clear();
}
