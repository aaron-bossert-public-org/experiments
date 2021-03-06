﻿
#include "vulkan/shader/vulkan_vertex_shader.h"

#include "vulkan/shader/vulkan_shader_impl.h"

using namespace igpu;


std::unique_ptr< vulkan_vertex_shader > vulkan_vertex_shader::make(
	const vulkan& vk,
	std::vector< uint32_t >&& memory )
{
	return vulkan_shader_impl_t< vulkan_vertex_shader >::make(
		vk,
		std::move( memory ) );
}
