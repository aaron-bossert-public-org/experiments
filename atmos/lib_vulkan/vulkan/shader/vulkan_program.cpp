
#include "vulkan/shader/vulkan_program.h"

#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/shader/vulkan_shader_stages.h"
#include "vulkan/shader/vulkan_vertex_shader.h"

#include "igpu/shader/program_parsing.h"

#include "framework/logging/log.h"

#include <algorithm>
#include <array>

using namespace igpu;

namespace
{
	void merge_parameters(
		const vulkan_shader& shader,
		std::array< std::array< uint8_t, 64 >, 3 >* out_indices,
		std::array< std::vector< spirv::parameter >, 3 >*
			descriptor_set_parameter_cfgs )
	{
		// Get all sampled images in the shader.
		for ( size_t i = 0; i < shader.parameter_count(); ++i )
		{
			const spirv::parameter& parameter = shader.parameter( i );
			auto& parameters =
				( *descriptor_set_parameter_cfgs )[parameter.spv
													   .descriptor_set];

			uint8_t* index = &( *out_indices )[parameter.spv.descriptor_set]
											  [parameter.spv.binding];
			if ( *index == (uint8_t)-1 )
			{
				*index = static_cast< uint8_t >( parameters.size() );

				parameters.push_back( parameter );
			}
			else
			{
				spirv::parameter* expect = &parameters[*index];

#define ERR_CHECK( MEMBER, FMT, F_OP )                             \
	if ( expect->MEMBER != parameter.MEMBER )                      \
	{                                                              \
		LOG_CRITICAL(                                              \
			"descriptor set %d binding %d previously seen as " FMT \
			" but now seen as " FMT,                               \
			(int)parameter.spv.descriptor_set,                     \
			(int)parameter.spv.binding,                            \
			F_OP( expect->MEMBER ),                                \
			F_OP( parameter.MEMBER ) );                            \
	}

				ERR_CHECK( name, "%s", []( const std::string& s ) {
					return s.c_str();
				} );
				ERR_CHECK( type, "%s", []( const igpu::parameter::type& t ) {
					return igpu::parameter::to_string( t ).data();
				} );
				ERR_CHECK( array_size, "%d", int );
#undef ERR_CHECK

				expect->spv.stages |= parameter.spv.stages;
			}
		}
	}
}

const vulkan_program::config& vulkan_program::cfg() const
{
	return _cfg;
}

size_t vulkan_program::parameter_count() const
{
	return _parameters.size();
}

const vulkan_parameter& vulkan_program::parameter( size_t i ) const
{
	return *_parameters[i];
}

size_t vulkan_program::batch_parameter_count() const
{
	return _batch_parameters.size();
}

size_t vulkan_program::vertex_parameter_count() const
{
	return _vertex_parameters.size();
}

const vulkan_vertex_parameter& vulkan_program::vertex_parameter(
	size_t i ) const
{
	return _vertex_parameters[i];
}

const vulkan_parameter& vulkan_program::batch_parameter( size_t i ) const
{
	return _batch_parameters[i];
}

size_t vulkan_program::material_parameter_count() const
{
	return _material_parameters.size();
}

const vulkan_parameter& vulkan_program::material_parameter( size_t i ) const
{
	return _material_parameters[i];
}

size_t vulkan_program::instance_parameter_count() const
{
	return _instance_parameters.size();
}

const vulkan_parameter& vulkan_program::instance_parameter( size_t i ) const
{
	return _instance_parameters[i];
}

const std::array< VkDescriptorSetLayout, 3 >& vulkan_program::
	descriptor_set_layouts() const
{
	return _descriptor_set_layouts;
}

VkPipelineLayout vulkan_program::pipeline_layout() const
{
	return _pipeline_layout;
}

std::unique_ptr< vulkan_program > vulkan_program::make( const config& cfg )
{
	// create merged list of shader uniform inputs
	std::array< std::vector< spirv::parameter >, 3 >
		descriptor_set_parameter_cfgs;
	std::array< std::array< uint8_t, 64 >, 3 > indices = {};
	memset( &indices, -1, sizeof indices );

	for ( vulkan_shader* shader : {
			  (vulkan_shader*)cfg.vk.vertex.get(),
			  (vulkan_shader*)cfg.vk.fragment.get(),
		  } )
	{
		merge_parameters( *shader, &indices, &descriptor_set_parameter_cfgs );
	}


	// create shader parameters, descriptor set layouts, and pipeline layout
	std::array< std::vector< vulkan_parameter >, 3 > descriptor_set_parameters;
	std::vector< vulkan_parameter* > parameters;
	std::array< VkDescriptorSetLayout, 3 > descriptor_set_layouts = {};
	VkPipelineLayout pipeline_layout = nullptr;
	std::vector< VkDescriptorSetLayoutBinding > scratch_bindings;


	parameters.reserve(
		descriptor_set_parameter_cfgs[0].size() +
		descriptor_set_parameter_cfgs[1].size() +
		descriptor_set_parameter_cfgs[2].size() );


	for ( size_t d = 0; d < 3; ++d )
	{
		auto& set_parameters = descriptor_set_parameters[d];
		auto& configs = descriptor_set_parameter_cfgs[d];

		set_parameters.reserve( configs.size() );
		scratch_bindings.resize( configs.size() );

		for ( size_t i = 0; i < configs.size(); ++i )
		{
			spirv::parameter& parameter = configs[i];
			set_parameters.emplace_back( parameter );
			parameters.emplace_back( &set_parameters.back() );

			auto& binding = scratch_bindings[i];
			binding.binding = parameter.spv.binding;
			binding.descriptorCount = (uint32_t)parameter.array_size;
			binding.descriptorType = to_vulkan_type( parameter.type );
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = to_vulkan_stage_flags( parameter.spv.stages );
		}

		VkDescriptorSetLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = (uint32_t)scratch_bindings.size();
		layout_info.pBindings = scratch_bindings.data();

		vkCreateDescriptorSetLayout(
			cfg.vk.device,
			&layout_info,
			nullptr,
			&descriptor_set_layouts[d] );
		scratch_bindings.clear();
	}

	// create pipeline layouts
	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount =
		(uint32_t)descriptor_set_layouts.size();
	pipeline_layout_info.pSetLayouts = &descriptor_set_layouts[0];

	vkCreatePipelineLayout(
		cfg.vk.device,
		&pipeline_layout_info,
		nullptr,
		&pipeline_layout );


	vulkan_vertex_shader& vertex_shader = *cfg.vk.vertex;
	std::vector< vulkan_vertex_parameter > vertex_parameters;
	vertex_parameters.reserve( vertex_shader.vertex_parameter_count() );
	for ( size_t i = 0; i < vertex_shader.vertex_parameter_count(); ++i )
	{
		vertex_parameters.emplace_back( vertex_shader.vertex_parameter( i ) );
	}

	return std::unique_ptr< vulkan_program >( new vulkan_program(
		cfg,
		std::move( descriptor_set_parameters[0] ),
		std::move( descriptor_set_parameters[1] ),
		std::move( descriptor_set_parameters[2] ),
		std::move( parameters ),
		std::move( vertex_parameters ),
		descriptor_set_layouts,
		pipeline_layout ) );
}

vulkan_program::~vulkan_program()
{
	vkDestroyPipelineLayout( _cfg.vk.device, _pipeline_layout, nullptr );

	for ( VkDescriptorSetLayout descriptor_set_layout :
		  _descriptor_set_layouts )
	{
		vkDestroyDescriptorSetLayout(
			_cfg.vk.device,
			descriptor_set_layout,
			nullptr );
	}
}

vulkan_program::vulkan_program(
	const config& cfg,
	std::vector< vulkan_parameter > batch_parameters,
	std::vector< vulkan_parameter > material_parameters,
	std::vector< vulkan_parameter > instance_parameters,
	std::vector< vulkan_parameter* > parameters,
	std::vector< vulkan_vertex_parameter > vertex_parameters,
	std::array< VkDescriptorSetLayout, 3 > descriptor_set_layouts,
	VkPipelineLayout pipeline_layout )
	: _cfg( cfg )
	, _batch_parameters( std::move( batch_parameters ) )
	, _material_parameters( std::move( material_parameters ) )
	, _instance_parameters( std::move( instance_parameters ) )
	, _parameters( std::move( parameters ) )
	, _vertex_parameters( std::move( vertex_parameters ) )
	, _descriptor_set_layouts( descriptor_set_layouts )
	, _pipeline_layout( pipeline_layout )
	, _texture_switch_metric(
		  perf::category::SWITCH_TEXTURES,
		  "Shader Texture Switches" )
	, _parameter_switch_metric(
		  perf::category::SWITCH_PARAMETERS,
		  "Shader parameter Switches" )
{}
