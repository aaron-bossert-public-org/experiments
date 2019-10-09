
 #pragma once

 #include <igpu/material/program.h>
 #include <framework/perf/metrics.h>
 #include <framework/utility/buffer_view.h>

 namespace igpu
 {
 	class batch;
     class batch_binding;
     class material;
     class vulkan_context;
     class vulkan_parameter;
 	class vulkan_vertex_parameter;
    
     class vulkan_program : public program
     {
     public:

         ~vulkan_program() override;

 		buffer_view<const parameter> batch_parameters() const override;

 		buffer_view<const parameter> material_parameters() const override;
 		
		buffer_view<const parameter> instance_parameters() const override;
 		
		size_t index_of_instance_parameter(const std::string_view&) const override;
 		
		const primitive& default_instance_primitive(size_t instance_parameter_index) const override;

         static std::unique_ptr<vulkan_program> make(
 			vulkan_context*,
 			const buffer_view<uint8_t>&,
 			const buffer_view<uint8_t>&);
                
     protected:

 		void update(const vulkan_parameter&, const primitive&) const;
        
 		vulkan_program(
 			vulkan_context*,
 			std::vector<vulkan_parameter> batch_parameters,
 			std::vector<vulkan_parameter> material_parameters,
 			std::vector<vulkan_parameter> instance_parameters,
 			std::unordered_map<std::string_view, size_t> instance_parameter_lookup);
        
     private:

 		vulkan_context* const _vulkan_context;
 		std::vector<vulkan_parameter> _batch_parameters;
 		std::vector<vulkan_parameter> _material_parameters;
 		std::vector<vulkan_parameter> _instance_parameters;
 		std::unordered_map<std::string_view, size_t> _instance_parameter_lookup;
 		perf::metric _texture_switch_metric;
 		perf::metric _parameter_switch_metric;
     };
 }
