
 #include <vulkan/buffer/vulkan_vertex_parameter.h>

 using namespace igpu;

 const vulkan_vertex_parameter::config& vulkan_vertex_parameter::cfg() const
 {
	 return _cfg;
 }

 vulkan_vertex_parameter::vulkan_vertex_parameter(const config& cfg)
 	: _cfg(cfg)
 {
 }
