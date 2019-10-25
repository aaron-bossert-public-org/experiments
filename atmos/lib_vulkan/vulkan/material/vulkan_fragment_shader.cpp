
#include <vulkan/material/vulkan_fragment_shader.h>
#include <vulkan/material/vulkan_shader.h>

using namespace igpu;


std::unique_ptr<vulkan_fragment_shader> vulkan_fragment_shader::make(VkDevice device)
{
	return vulkan_shader_t<vulkan_fragment_shader>::make({
		device,
		VK_SHADER_STAGE_FRAGMENT_BIT });
}
