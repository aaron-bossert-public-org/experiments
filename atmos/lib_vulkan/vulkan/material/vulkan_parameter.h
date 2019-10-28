
 #pragma once

#include <vulkan/defines/vulkan_includes.h>

 #include <framework/meta_programming/va_macro.h>
 #include <igpu/material/parameter.h>

 #include <optional>

 namespace igpu
 {
 	class vulkan_parameter : public parameter
 	{
 	public:

		struct config : parameter::config
		{
			const size_t descriptor_set = (size_t)-1;
			VkShaderStageFlags stage_flags;
		};

		const config& cfg() const override;

 		vulkan_parameter(
 			const config&);

 	private:

		const config _cfg;
 	};
 }
