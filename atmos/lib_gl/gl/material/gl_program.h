
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
    
    class gl_program : public program
    {
    public:

		struct config : program::config
		{
			gl_context* context;
		};

		const config& cfg() const override;

		size_t batch_parameter_count() const override;
		const parameter& batch_parameter(size_t) const override;

		size_t material_parameter_count() const override;
		const parameter& material_parameter(size_t) const override;

		size_t instance_parameter_count() const override;
		const parameter& instance_parameter(size_t) const override;

		size_t index_of_instance_parameter(const std::string_view&) const override;
		
		const primitive& default_instance_primitive(size_t instance_parameter_index) const override;

        ~gl_program() override;
		
		unsigned gl_handle() const;

		void update(size_t batch_drawpass_id, const batch*);

		void update(const material*);

		void update(const batch_binding*);

        static std::unique_ptr<gl_program> make(
			const config&);
                
    protected:

		void update(const gl_parameter&, const primitive&) const;
        
		gl_program(
			const config&,
			unsigned gl_handle,
			std::vector<gl_parameter> batch_parameters,
			std::vector<gl_parameter> material_parameters,
			std::vector<gl_parameter> instance_parameters,
			std::unordered_map<std::string_view, size_t> instance_parameter_lookup);
        
    private:
		const config _cfg;
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
