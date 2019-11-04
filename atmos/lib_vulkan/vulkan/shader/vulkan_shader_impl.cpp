
#include "vulkan/shader/vulkan_vertex_shader.h"
#include "vulkan/shader/vulkan_shader_impl.h"
#include "vulkan/shader/vulkan_compiler.h"
#include "vulkan/shader/vulkan_shader_stages.h"

#include "igpu/shader/program_parsing.h"

using namespace igpu;

namespace
{
	const std::string s_entry_point = "main";
}

vulkan_shader_impl::vulkan_shader_impl(
	const vulkan_shader::vulkan& vk,
	vector_buffer<uint32_t>&& buffer)
: _vk(vk)
, _cpu_mem_metric(perf::category::CPU_MEM_USAGE, "spirv buffer")
{
	buffer_view<uint32_t> view_unmapped = buffer.view_unmapped();
	VkShaderModuleCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	create_info.codeSize = view_unmapped.byte_size();
	create_info.pCode = (uint32_t*)view_unmapped.data();

	vkCreateShaderModule(_vk.device, &create_info, nullptr, &_shader_module);

	_parameters.clear();
	_vertex_parameters.clear();

	shader_stages stages = from_vulkan_stage_flags(_vk.stage_flags);

	spirv::parse(std::move(buffer.release()), s_entry_point, stages, &_parameters, &_vertex_parameters);
}

vulkan_shader_impl::~vulkan_shader_impl()
{
	if (_shader_module)
	{
		// it is safe to destroy shader modules while they are in use
		vkDestroyShaderModule(_vk.device, _shader_module, nullptr);
	}

	_parameters.clear();
	_vertex_parameters.clear();
}

const vulkan_shader::vulkan& vulkan_shader_impl::vk() const
{
	return _vk;
}

VkShaderModule vulkan_shader_impl::shader_module() const
{
	return _shader_module;
}

VkPipelineShaderStageCreateInfo vulkan_shader_impl::stage_info() const
{
	VkPipelineShaderStageCreateInfo shader_stage_info = {};
	shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_info.stage = _vk.stage_flags;
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
