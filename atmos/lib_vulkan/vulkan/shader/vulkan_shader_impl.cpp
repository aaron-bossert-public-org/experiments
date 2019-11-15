
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

vulkan_shader_impl::vulkan_shader_impl( const vulkan_shader::vulkan& vk )
	: _vk( vk )
	, _cpu_mem_metric( perf::category::CPU_MEM_USAGE, "spirv buffer" )
{}

vulkan_shader_impl::~vulkan_shader_impl()
{
	release();
}

const vulkan_shader::vulkan& vulkan_shader_impl::vk() const
{
	return _vk;
}

VkShaderModule vulkan_shader_impl::shader_module() const
{
	return _shader_module;
}

void vulkan_shader_impl::map( buffer_view_base* out_mapped_view )
{
	if ( _memory.size() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		_memory.resize( ( out_mapped_view->byte_size() + 3 ) / 4 );

		_cpu_mem_metric.reset();
		_cpu_mem_metric.add( _memory.size() * sizeof( _memory[0] ) );

		*out_mapped_view = buffer_view_base(
			out_mapped_view->size(),
			_memory.data(),
			out_mapped_view->stride() );
	}
}

void vulkan_shader_impl::unmap()
{
	if ( !_memory.size() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		release();

		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = _memory.size() * sizeof _memory[0];
		create_info.pCode = _memory.data();

		vkCreateShaderModule(
			_vk.device,
			&create_info,
			nullptr,
			&_shader_module );

		_parameters.clear();
		_vertex_parameters.clear();

		shader_stages stages =
			from_vulkan_shader_stage_flags( _vk.stage_flags );

		if ( false ==
			 spirv_parse(
				 std::move( _memory ),
				 s_entry_point,
				 stages,
				 &_parameters,
				 &_vertex_parameters ) )
		{
			_parameters.clear();
			_vertex_parameters.clear();
		}
	}
}

VkPipelineShaderStageCreateInfo vulkan_shader_impl::stage_info() const
{
	VkPipelineShaderStageCreateInfo shader_stage_info = {};
	shader_stage_info.sType =
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage_info.stage = _vk.stage_flags;
	shader_stage_info.module = _shader_module;
	shader_stage_info.pName = s_entry_point.c_str();
	return shader_stage_info;
}

size_t vulkan_shader_impl::parameter_count() const
{
	return _parameters.size();
}

const parameter::config& vulkan_shader_impl::parameter( size_t i ) const
{
	return _parameters[i];
}

size_t vulkan_shader_impl::vertex_parameter_count() const
{
	return _vertex_parameters.size();
}

const vertex_parameter::config& vulkan_shader_impl::vertex_parameter(
	size_t i ) const
{
	return _vertex_parameters[i];
}

void vulkan_shader_impl::release()
{
	if ( _shader_module )
	{
		// it is safe to destroy shader modules while they are in use
		vkDestroyShaderModule( _vk.device, _shader_module, nullptr );
	}

	_parameters.clear();
	_vertex_parameters.clear();
}
