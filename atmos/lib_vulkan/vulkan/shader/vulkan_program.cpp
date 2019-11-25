
#include "vulkan/shader/vulkan_program.h"

#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_program_impl.h"
#include "vulkan/shader/vulkan_vertex_shader.h"

#include "framework/logging/log.h"

using namespace igpu;

const vulkan_program::config& vulkan_program::cfg() const
{
	return _cfg;
}

const vulkan_vertex_parameters& vulkan_program::vertex_parameters() const
{
	return _vertex_parameters;
}

std::unique_ptr< vulkan_program > vulkan_program::make( const config& cfg )
{
	vulkan_program_impl::internal_config internal_cfg;
	vulkan_program_impl::config impl_cfg = {
		cfg.vk.device,
		{ cfg.vk.vertex, cfg.vk.fragment } };

	if ( vulkan_program_impl::make_internal_cfg( impl_cfg, &internal_cfg ) )
	{
		vulkan_vertex_shader& vertex_shader = *cfg.vk.vertex;
		std::vector< vulkan_vertex_parameter > vertex_parameters;
		vertex_parameters.reserve( vertex_shader.vertex_parameter_count() );
		for ( size_t i = 0; i < vertex_shader.vertex_parameter_count(); ++i )
		{
			vertex_parameters.emplace_back(
				vertex_shader.vertex_parameter( i ) );
		}

		return std::unique_ptr< vulkan_program >(
			new vulkan_program_impl_t< vulkan_program >(
				impl_cfg,
				std::move( internal_cfg ),
				cfg,
				std::move( vertex_parameters ) ) );
	}

	return nullptr;
}

vulkan_program::~vulkan_program()
{}

vulkan_program::vulkan_program(
	const config& cfg,
	std::vector< vulkan_vertex_parameter >&& vertex_parameters )
	: _cfg( cfg )
	, _vertex_parameters( std::move( vertex_parameters ) )
{}
