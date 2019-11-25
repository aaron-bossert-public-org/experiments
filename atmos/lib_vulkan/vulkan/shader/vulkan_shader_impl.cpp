
#include "vulkan/shader/vulkan_shader_impl.h"

#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/shader/vulkan_vertex_shader.h"

#include "igpu/shader/program_parsing.h"

#include "framework/utility/buffer_view.h"

using namespace igpu;

namespace
{
	const std::string s_entry_point = "main";
}

vulkan_shader_impl::vulkan_shader_impl(
	const vulkan_shader::vulkan& vk,
	state&& st )
	: _vk( vk )
	, _st( std::move( st ) )
{}

vulkan_shader_impl::~vulkan_shader_impl()
{
	if ( _st.shader_module )
	{
		// it is safe to destroy shader modules while they are in use
		vkDestroyShaderModule( _vk.device, _st.shader_module, nullptr );
	}
}

const vulkan_shader::vulkan& vulkan_shader_impl::vk() const
{
	return _vk;
}

const vulkan_shader_impl::state& vulkan_shader_impl::st() const
{
	return _st;
}

VkPipelineShaderStageCreateInfo vulkan_shader_impl::stage_info() const
{
	VkPipelineShaderStageCreateInfo shader_stage_info = {};
	shader_stage_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_info.stage = _vk.stage_flags;
	shader_stage_info.module = _st.shader_module;
	shader_stage_info.pName = s_entry_point.c_str();
	return shader_stage_info;
}

bool vulkan_shader_impl::make_state(
	const vulkan_shader::vulkan& vk,
	std::vector< uint32_t >&& memory,
	state* out_st )
{
	if ( !memory.size() )
	{
		LOG_CRITICAL( "memory is empty" );
	}
	else
	{
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = memory.size() * sizeof memory[0];
		create_info.pCode = memory.data();

		vkCreateShaderModule(
			vk.device,
			&create_info,
			nullptr,
			&out_st->shader_module );
		if ( out_st->shader_module )
		{
			shader_stages stages =
				from_vulkan_shader_stage_flags( vk.stage_flags );

			if ( spirv_parse(
					 std::move( memory ),
					 s_entry_point,
					 stages,
					 &out_st->constants,
					 &out_st->parameters,
					 &out_st->vertex_parameters ) )
			{
				return true;
			}
		}
	}
	if ( out_st->shader_module )
	{
		vkDestroyShaderModule( vk.device, out_st->shader_module, nullptr );
	}
	return false;
}