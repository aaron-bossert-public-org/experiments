
#include "vulkan/shader/vulkan_program_impl.h"

#include "vulkan/compute/vulkan_compute_shader.h"
#include "vulkan/shader/vulkan_parameter.h"

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

				ERR_CHECK( name, "%s", []( const auto& s ) {
					return s.c_str();
				} );
				ERR_CHECK( type, "%s", []( const auto& t ) {
					return igpu::parameter::to_string( t ).data();
				} );
				ERR_CHECK( array_size, "%d", int );
#undef ERR_CHECK

				expect->shader_stages |= parameter.shader_stages;
				expect->decorators |= parameter.decorators;
			}
		}
	}

	void merge_parameters(
		const vulkan_shader& shader,
		std::array< uint8_t, constant_parameters::MAX_COUNT >* out_indices,
		std::vector< constant_parameter::config >* constant_cfgs )
	{
		// Get all sampled images in the shader.
		for ( size_t i = 0; i < shader.constant_count(); ++i )
		{
			const constant_parameter::config& constant = shader.constant( i );

			uint8_t* index = &( *out_indices )[constant.binding];
			if ( *index == (uint8_t)-1 )
			{
				*index = static_cast< uint8_t >( constant_cfgs->size() );

				constant_cfgs->push_back( constant );
			}
			else
			{
				constant_parameter::config* expect =
					&constant_cfgs->at( *index );

#define ERR_CHECK( MEMBER, FMT, F_OP )                             \
	if ( expect->MEMBER != constant.MEMBER )                       \
	{                                                              \
		LOG_CRITICAL(                                              \
			"descriptor set %d binding %d previously seen as " FMT \
			" but now seen as " FMT,                               \
			(int)constant.binding,                                 \
			F_OP( expect->MEMBER ),                                \
			F_OP( constant.MEMBER ) );                             \
	}

				ERR_CHECK( constant.name, "%s", []( auto& s ) {
					return s.c_str();
				} );
				ERR_CHECK( constant.type(), "%s", []( const auto& t ) {
					return igpu::to_string( t ).data();
				} );
#undef ERR_CHECK

				expect->shader_stages |= constant.shader_stages;
			}
		}
	}
}

const vulkan_program_impl::config& vulkan_program_impl::cfg() const
{
	return _cfg;
}

const vulkan_constant_parameters& vulkan_program_impl::constant_parameters()
	const
{
	return _constant_parameters;
}

const vulkan_parameters& vulkan_program_impl::batch_parameters() const
{
	return _batch_parameters;
}

const vulkan_parameters& vulkan_program_impl::material_parameters() const
{
	return _material_parameters;
}

const vulkan_parameters& vulkan_program_impl::instance_parameters() const
{
	return _instance_parameters;
}

VkPipelineLayout vulkan_program_impl::pipeline_layout() const
{
	return _pipeline_layout;
}

bool vulkan_program_impl::make_internal_cfg(
	const config& cfg,
	internal_config* out_internal_config )
{
	// create merged list of shader uniform inputs
	std::array< std::vector< parameter::config >, 3 > base_parameter_cfgs;
	std::array< std::array< uint8_t, parameters::MAX_COUNT >, 3 >
		parameter_indices = {};
	memset( &parameter_indices, -1, sizeof parameter_indices );

	constant_parameters::config constant_cfgs;
	std::array< uint8_t, constant_parameters::MAX_COUNT > constant_indices = {};
	memset( &constant_indices, -1, sizeof constant_indices );

	bool shaders_valid = true;
	for ( const auto& shader : cfg.shaders )
	{
		if ( !shader )
		{
			LOG_CRITICAL( "shader is null" );
		}
		else if ( !shader->shader_module() )
		{
			LOG_CRITICAL( "shader module has not been loaded" );
		}
		else
		{
			merge_parameters(
				*shader,
				&parameter_indices,
				&base_parameter_cfgs );

			merge_parameters(
				*shader,
				&constant_indices,
				&constant_cfgs.constants );

			continue;
		}

		shaders_valid = false;
	}

	if ( !shaders_valid )
	{
		return false;
	}

	// create shader parameters, descriptor set layouts, and pipeline layout
	std::array< vulkan_parameters::config, 3 > parameters_cfgs;
	VkPipelineLayout pipeline_layout = nullptr;
	std::vector< VkDescriptorSetLayoutBinding > scratch_bindings;

	for ( size_t d = 0; d < 3; ++d )
	{
		auto& parameters_cfg = parameters_cfgs[d];
		auto& configs = base_parameter_cfgs[d];
		parameters_cfg.vk.device = cfg.device;
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
			cfg.device,
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
		cfg.device,
		&pipeline_layout_info,
		nullptr,
		&pipeline_layout );

	out_internal_config->constant_parameters = std::move( constant_cfgs );
	out_internal_config->batch_parameters = std::move( parameters_cfgs[0] );
	out_internal_config->material_parameters = std::move( parameters_cfgs[1] );
	out_internal_config->instance_parameters = std::move( parameters_cfgs[2] );
	out_internal_config->pipeline_layout = pipeline_layout;

	return true;
}

vulkan_program_impl::~vulkan_program_impl()
{
	vkDestroyPipelineLayout( _cfg.device, _pipeline_layout, nullptr );
}

vulkan_program_impl::vulkan_program_impl(
	const config& cfg,
	internal_config&& internal_cfg )
	: _cfg( cfg )
	, _constant_parameters( std::move( internal_cfg.constant_parameters ) )
	, _batch_parameters( std::move( internal_cfg.batch_parameters ) )
	, _material_parameters( std::move( internal_cfg.material_parameters ) )
	, _instance_parameters( std::move( internal_cfg.instance_parameters ) )
	, _pipeline_layout( internal_cfg.pipeline_layout )
{}
