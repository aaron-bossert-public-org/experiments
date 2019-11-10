
#include "vulkan/shader/vulkan_program.h"

#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_parameter.h"
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
		std::array< std::array< uint8_t, parameters::MAX_COUNT >, 3 >*
			out_indices,
		std::array< std::vector< parameter::config >, 3 >* parameter_cfgs )
	{
		// Get all sampled images in the shader.
		for ( size_t i = 0; i < shader.parameter_count(); ++i )
		{
			const parameter::config& parameter = shader.parameter( i );
			auto& parameters = ( *parameter_cfgs )[parameter.descriptor_set];

			uint8_t* index =
				&( *out_indices )[parameter.descriptor_set][parameter.binding];
			if ( *index == (uint8_t)-1 )
			{
				*index = static_cast< uint8_t >( parameters.size() );

				parameters.push_back( parameter );
			}
			else
			{
				parameter::config* expect = &parameters[*index];

#define ERR_CHECK( MEMBER, FMT, F_OP )                             \
	if ( expect->MEMBER != parameter.MEMBER )                      \
	{                                                              \
		LOG_CRITICAL(                                              \
			"descriptor set %d binding %d previously seen as " FMT \
			" but now seen as " FMT,                               \
			(int)parameter.descriptor_set,                         \
			(int)parameter.binding,                                \
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

				expect->shader_stages |= parameter.shader_stages;
				expect->decorators |= parameter.decorators;
			}
		}
	}
}

const vulkan_program::config& vulkan_program::cfg() const
{
	return _cfg;
}

const vulkan_parameters& vulkan_program::batch_parameters() const
{
	return _batch_parameters;
}

const vulkan_parameters& vulkan_program::material_parameters() const
{
	return _material_parameters;
}

const vulkan_parameters& vulkan_program::instance_parameters() const
{
	return _instance_parameters;
}

const vulkan_vertex_parameters& vulkan_program::vertex_parameters() const
{
	return _vertex_parameters;
}

VkPipelineLayout vulkan_program::pipeline_layout() const
{
	return _pipeline_layout;
}


std::unique_ptr< vulkan_program > vulkan_program::make( const config& cfg )
{
	// create merged list of shader uniform inputs
	std::array< std::vector< parameter::config >, 3 > base_parameter_cfgs;
	std::array< std::array< uint8_t, parameters::MAX_COUNT >, 3 > indices = {};
	memset( &indices, -1, sizeof indices );

	vulkan_shader* raw_shaders[] = {
		cfg.vk.vertex.get(),
		cfg.vk.fragment.get(),
	};

	for ( vulkan_shader* shader : raw_shaders )
	{
		merge_parameters( *shader, &indices, &base_parameter_cfgs );
	}

	// create shader parameters, descriptor set layouts, and pipeline layout
	std::array< vulkan_parameters::config, 3 > parameters_cfgs;
	VkPipelineLayout pipeline_layout = nullptr;
	std::vector< VkDescriptorSetLayoutBinding > scratch_bindings;

	for ( size_t d = 0; d < 3; ++d )
	{
		auto& parameters_cfg = parameters_cfgs[d];
		auto& configs = base_parameter_cfgs[d];
		parameters_cfg.vk.device = cfg.vk.device;
		parameters_cfg.vk.parameters.reserve( configs.size() );
		scratch_bindings.resize( configs.size() );

		for ( size_t i = 0; i < configs.size(); ++i )
		{
			parameters_cfg.vk.parameters.emplace_back( configs[i] );
			const vulkan_parameter::config& parameter =
				parameters_cfg.vk.parameters.back().cfg();

			auto& binding = scratch_bindings[i];
			binding.binding = (uint32_t)parameter.binding;
			binding.descriptorCount = (uint32_t)parameter.array_size;
			binding.descriptorType = parameter.vk.descriptor_type;
			binding.pImmutableSamplers = nullptr;
			binding.stageFlags = parameter.vk.shader_stages;
		}

		VkDescriptorSetLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = (uint32_t)scratch_bindings.size();
		layout_info.pBindings = scratch_bindings.data();

		vkCreateDescriptorSetLayout(
			cfg.vk.device,
			&layout_info,
			nullptr,
			&parameters_cfg.vk.layout );
		scratch_bindings.clear();
	}

	// create pipeline layouts
	VkPipelineLayoutCreateInfo pipeline_layout_info = {};
	std::array< VkDescriptorSetLayout, 3 > layouts = {
		parameters_cfgs[0].vk.layout,
		parameters_cfgs[1].vk.layout,
		parameters_cfgs[2].vk.layout,
	};

	ASSERT_CONTEXT( layouts.size() == parameters_cfgs.size() );
	pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_info.setLayoutCount = (uint32_t)parameters_cfgs.size();
	pipeline_layout_info.pSetLayouts = layouts.data();

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
		pipeline_layout,
		std::move( vertex_parameters ),
		std::move( parameters_cfgs[0] ),
		std::move( parameters_cfgs[1] ),
		std::move( parameters_cfgs[2] ) ) );
}

vulkan_program::~vulkan_program()
{
	vkDestroyPipelineLayout( _cfg.vk.device, _pipeline_layout, nullptr );
}

vulkan_program::vulkan_program(
	const config& cfg,
	VkPipelineLayout pipeline_layout,
	std::vector< vulkan_vertex_parameter >&& vertex_parameters,
	vulkan_parameters::config&& batch_cfg,
	vulkan_parameters::config&& material_cfg,
	vulkan_parameters::config&& instance_cfg )
	: _cfg( cfg )
	, _pipeline_layout( pipeline_layout )
	, _vertex_parameters( std::move( vertex_parameters ) )
	, _batch_parameters( std::move( batch_cfg ) )
	, _material_parameters( std::move( material_cfg ) )
	, _instance_parameters( std::move( instance_cfg ) )
	, _texture_switch_metric(
		  perf::category::SWITCH_TEXTURES,
		  "Shader Texture Switches" )
	, _parameter_switch_metric(
		  perf::category::SWITCH_PARAMETERS,
		  "Shader parameter Switches" )
{}
