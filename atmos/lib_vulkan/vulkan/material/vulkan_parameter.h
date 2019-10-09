
 #pragma once

 #include <framework/meta_programming/va_macro.h>
 #include <igpu/material/parameter.h>

 #include <optional>

 namespace igpu
 {
 	class vulkan_parameter : public parameter
 	{
 	public:

 		vulkan_parameter();

 		vulkan_parameter(
 			const config&,
 			size_t constraint_index);

 		vulkan_parameter(const vulkan_parameter&) = default;

 		vulkan_parameter& operator= (const vulkan_parameter&) = default;

 	private:
 	};
 }
