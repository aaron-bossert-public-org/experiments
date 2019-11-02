
#include <vulkan/shader/vulkan_parameter.h>

using namespace igpu;

VkDescriptorType igpu::to_vulkan_type(parameter::type type)
{
	switch (type)
	{
	case parameter::type::COMPUTE_BUFFER:
		return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	case parameter::type::TEXTURE2D:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	};

	if (parameter::is_valid(type))
	{
		LOG_CRITICAL("unhandled type: %s", parameter::to_string(type).data());
	}
	else
	{
		LOG_CRITICAL("invalid type: %d", type);
	}

	return (VkDescriptorType)-1;

}

const vulkan_parameter::config& vulkan_parameter::cfg() const
{
	return _cfg;
}

vulkan_parameter::vulkan_parameter(const spirv::parameter& cfg)
	: _cfg{ cfg }
{
}
