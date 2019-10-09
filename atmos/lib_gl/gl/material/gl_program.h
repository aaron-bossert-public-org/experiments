
#pragma once

#include <igpu/material/program.h>
#include <framework/perf/metrics.h>
#include <framework/utility/buffer_view.h>

namespace igpu
{
	class batch;
    class batch_binding;
    class material;
    class gl_context;
    class gl_parameter;
	class gl_vertex_parameter;
    
    class gl_program : public program
    {
    public:

		buffer_view<const parameter> batch_parameters() const override;
		
		buffer_view<const parameter> material_parameters() const override;
		
		buffer_view<const parameter> instance_parameters() const override;
		
		size_t index_of_instance_parameter(const std::string_view&) const override;
		
		const primitive& default_instance_primitive(size_t instance_parameter_index) const override;

        ~gl_program() override;
		
		unsigned gl_handle() const;

		void update(size_t batch_drawpass_id, const batch*);

		void update(const material*);

		void update(const batch_binding*);

        static std::unique_ptr<gl_program> make(
			gl_context*,
			const buffer_view<uint8_t>& vertex_code,
			const buffer_view<uint8_t>& pixel_code);
                
    protected:

		void update(const gl_parameter&, const primitive&) const;
        
		gl_program(
			gl_context*,
			unsigned gl_handle,
			std::vector<gl_parameter> batch_parameters,
			std::vector<gl_parameter> material_parameters,
			std::vector<gl_parameter> instance_parameters,
			std::unordered_map<std::string_view, size_t> instance_parameter_lookup);
        
    private:

		gl_context* const _gl_context;
		const unsigned _gl_handle;
		size_t _batch_drawpass_id = 0;
		std::vector<gl_parameter> _batch_parameters;
		std::vector<gl_parameter> _material_parameters;
		std::vector<gl_parameter> _instance_parameters;
		std::unordered_map<std::string_view, size_t> _instance_parameter_lookup;
		perf::metric _texture_switch_metric;
		perf::metric _parameter_switch_metric;
    };
}
