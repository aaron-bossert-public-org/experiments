
#pragma once

#include "gl/buffer/gl_vertex_parameter.h"
#include "gl/material/gl_parameter.h"

#include "igpu/shader/program.h"

#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"

namespace igpu
{
	class batch;
	class batch_binding;
	class parameter;
	class primitives;
	class gl_context;

	class gl_program : public program
	{
	public:
		struct config : program::config
		{
			struct gl
			{
				gl_context* context = nullptr;
			};

			gl gl;
		};

		const config& cfg() const override;

		size_t parameter_count() const override;

		const gl_parameter& parameter( size_t ) const override;

		size_t vertex_parameter_count() const override;

		const gl_vertex_parameter& vertex_parameter( size_t ) const override;

		~gl_program() override;

		unsigned gl_handle() const;

		void update( size_t batch_drawpass_id, const batch* );

		void update_material( const primitives* );

		void update( const batch_binding* );

		static std::unique_ptr< gl_program > make( const config& );

	protected:
		void update( const gl_parameter&, const primitive& ) const;

		gl_program(
			const config&,
			unsigned gl_handle,
			std::vector< gl_parameter > batch_parameters,
			std::vector< gl_parameter > material_parameters,
			std::vector< gl_parameter > instance_parameters,
			std::unordered_map< std::string_view, size_t >
				instance_parameter_lookup );

	private:
		const config _cfg;
		const unsigned _gl_handle;
		size_t _batch_drawpass_id = 0;
		std::vector< gl_parameter > _batch_parameters;
		std::vector< gl_parameter > _material_parameters;
		std::vector< gl_parameter > _instance_parameters;
		std::unordered_map< std::string_view, size_t >
			_instance_parameter_lookup;
		perf::metric _texture_switch_metric;
		perf::metric _parameter_switch_metric;
	};
}
