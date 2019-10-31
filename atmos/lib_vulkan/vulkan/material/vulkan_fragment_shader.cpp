
#include <vulkan/material/vulkan_fragment_shader.h>
#include <vulkan/material/vulkan_shader_impl.h>

using namespace igpu;


std::unique_ptr<vulkan_fragment_shader> vulkan_fragment_shader::make(
	const config& cfg)
{
	return vulkan_shader_impl_t<vulkan_fragment_shader>::make(
		cfg);
}
