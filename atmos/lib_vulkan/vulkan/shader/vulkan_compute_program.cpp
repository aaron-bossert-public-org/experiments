
#include "vulkan/shader/vulkan_compute_program.h"

#include "vulkan/shader/vulkan_compute_shader.h"
#include "vulkan/shader/vulkan_program_impl.h"

#include "framework/logging/log.h"

using namespace igpu;

const vulkan_compute_program::config& vulkan_compute_program::cfg() const
{
	return _cfg;
}

std::unique_ptr< vulkan_compute_program > vulkan_compute_program::make(
	const config& cfg )
{
	vulkan_program_impl::internal_config internal_cfg;
	vulkan_program_impl::config impl_cfg = {
		cfg.vk.device,
		{ cfg.vk.compute } };

	if ( vulkan_program_impl::make_internal_cfg( impl_cfg, &internal_cfg ) )
	{
		return std::unique_ptr< vulkan_compute_program >(
			new vulkan_program_impl_t< vulkan_compute_program >(
				impl_cfg,
				std::move( internal_cfg ),
				cfg ) );
	}

	return nullptr;
}

vulkan_compute_program::~vulkan_compute_program()
{}

vulkan_compute_program::vulkan_compute_program( const config& cfg )
	: _cfg( cfg )
{}
