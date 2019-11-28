
#include "igpu/compute/compute_pipeline.h"

#include "igpu/compute/compute_program.h"
#include "igpu/shader/constant_parameters.h"

#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

compute_pipeline::config compute_pipeline::make_config(
	const constants::config& constants,
	const scoped_ptr< compute_program >& program )
{
	compute_pipeline::config cfg{
		program,
	};

	const auto& constant_params =
		program->constant_parameters().cfg().constants;
	for ( size_t i = 0; i < constant_params.size(); ++i )
	{
		const constant* found = nullptr;
		const constant_parameter::config& param = constant_params[i];

		for ( const constants::config* constant_cfgs : {
				  &constants,
				  &program->cfg().constants,
			  } )
		{
			for ( const constant& compare : *constant_cfgs )
			{
				if ( compare.name == param.constant.name )
				{
					found = &compare;
					cfg.compact_constants.push_back( {
						compare,
						param.binding,
						param.shader_stages,
					} );

					break;
				}
			}
			if ( found )
			{
				break;
			}
		}
	}

	return cfg;
}

size_t compute_pipeline::config::hash() const
{
	size_t h = hash_utils::hash( program );

	for ( const auto& constant_cfg : compact_constants )
	{
		h = hash_utils::hash_combine( h, constant_cfg.constant.hash() );
	}

	return h;
}

ptrdiff_t compute_pipeline::config::compare( const config& other ) const
{
	if ( auto cmp = program.get() - other.program.get() )
	{
		return cmp;
	}

	size_t const_count =
		std::min( compact_constants.size(), other.compact_constants.size() );
	for ( size_t i = 0; i < const_count; ++i )
	{
		const constant_parameter::config& l_cfg = compact_constants[i];
		const constant_parameter::config& r_cfg = other.compact_constants[i];

		if ( auto cmp = l_cfg.binding - r_cfg.binding )
		{
			return cmp;
		}

		if ( auto cmp = l_cfg.constant.compare( r_cfg.constant ) )
		{
			return cmp;
		}
	}

	return compact_constants.size() - other.compact_constants.size();
}
