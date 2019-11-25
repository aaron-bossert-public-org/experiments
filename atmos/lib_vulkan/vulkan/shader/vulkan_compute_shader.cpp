
#include "vulkan/shader/vulkan_compute_shader.h"

#include "vulkan/shader/vulkan_shader_impl.h"

using namespace igpu;


std::unique_ptr< vulkan_compute_shader > vulkan_compute_shader::make(
	const vulkan& vk,
	std::vector< uint32_t >&& memory )
{
	return vulkan_shader_impl_t< vulkan_compute_shader >::make(
		vk,
		std::move( memory ) );
}
