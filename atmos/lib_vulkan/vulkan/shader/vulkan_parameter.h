
#pragma once

#include <vulkan/defines/vulkan_includes.h>

#include <framework/meta_programming/va_macro.h>
#include <igpu/shader/program_parsing.h>

#include <optional>

namespace igpu
{
	VkDescriptorType to_vulkan_type(parameter::type);
	class vulkan_parameter : public parameter
	{
	public:

		struct config : spirv::parameter
		{
		};
		
		const config& cfg() const override;

		vulkan_parameter(
			const spirv::parameter&);

	private:

		const config _cfg;
	};
}
