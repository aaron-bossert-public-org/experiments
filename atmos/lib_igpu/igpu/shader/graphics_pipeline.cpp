
#include "igpu/shader/graphics_pipeline.h"

#include "igpu/buffer/geometry.h"
#include "igpu/shader/attribute_indexer.h"
#include "igpu/shader/constant_parameters.h"
#include "igpu/shader/program.h"
#include "igpu/shader/render_states.h"
#include "igpu/shader/vertex_parameters.h"
#include "igpu/texture/draw_target.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

graphics_pipeline::config graphics_pipeline::make_config(
	const constants::config& batch_constants,
	const attribute_indexer& indexer,
	const scoped_ptr< draw_target >& draw_target,
	const scoped_ptr< program >& program,
	const scoped_ptr< render_states >& render_states )
{
	graphics_pipeline::config cfg{
		draw_target,
		program,
		render_states,
		indexer.geometry()->cfg().topology,
		std::vector< vertex_buffer::config >( indexer.buffer_count() ),
	};

	// build one vertex buffer config for each final buffer specified by the
	// indexer. this can be a subset of the buffers in the geometry provided to
	// the indexer.
	for ( size_t i = 0; i < indexer.buffer_count(); ++i )
	{
		size_t buffer_index = indexer.buffer_indices()[i];
		auto* buffer_cfg = &cfg.compact_vertex_format[i];
		auto& ref_cfg = indexer.geometry()->vertex_buffer( buffer_index ).cfg();
		buffer_cfg->memory = ref_cfg.memory;
		buffer_cfg->stride = ref_cfg.stride;
	}

	// build one vertex attribute config for each attribute specified by the
	// indexer. this can be a subset of the attributes in the vertex buffers of
	// the geometry provided to the indexer.
	for ( size_t i = 0; i < indexer.vertex_parameters()->count(); ++i )
	{
		const attribute_indexer::attribute_source& source =
			indexer.indices()[i];

		size_t buffer_index =
			indexer.buffer_indices()[source.compact_buffer_index];

		const vertex_buffer::config& ref_buff_cfg =
			indexer.geometry()->vertex_buffer( buffer_index ).cfg();

		const vertex_buffer::attribute& attribute =
			ref_buff_cfg.attributes[source.attribute_index];

		cfg.compact_vertex_format[source.compact_buffer_index]
			.attributes.push_back( attribute );
	}

	const auto& constant_params =
		program->constant_parameters().cfg().constants;
	for ( size_t i = 0; i < constant_params.size(); ++i )
	{
		const constant* found = nullptr;
		const constant_parameter::config& param = constant_params[i];

		for ( const constants::config* constant_cfgs : {
				  &indexer.geometry()->cfg().constants,
				  &program->cfg().constants,
				  &batch_constants,
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

size_t graphics_pipeline::config::hash() const
{
	size_t h = hash_utils::hash_combine(
		draw_target,
		program,
		render_states,
		(size_t)topology );

	for ( const auto& buffer_config : compact_vertex_format )
	{
		h = hash_utils::hash_combine( h, buffer_config.stride );
		for ( const auto& attribute : buffer_config.attributes )
		{
			h = hash_utils::hash_combine(
				h,
				attribute.name,
				(size_t)attribute.components,
				(size_t)attribute.offset );
		}
	}

	for ( const auto& constant_cfg : compact_constants )
	{
		h = hash_utils::hash_combine( h, constant_cfg.constant.hash() );
	}

	return h;
}

ptrdiff_t graphics_pipeline::config::compare( const config& other ) const
{
	if ( auto cmp = draw_target.get() - other.draw_target.get() )
	{
		return cmp;
	}

	if ( auto cmp = program.get() - other.program.get() )
	{
		return cmp;
	}

	if ( auto cmp = render_states.get() - other.render_states.get() )
	{
		return cmp;
	}

	if ( auto cmp = (ptrdiff_t)topology - (ptrdiff_t)other.topology )
	{
		return cmp;
	}

	size_t compare_buffer_count = std::min(
		compact_vertex_format.size(),
		other.compact_vertex_format.size() );

	for ( size_t b = 0; b < compare_buffer_count; ++b )
	{
		const vertex_buffer::config& l_buff = compact_vertex_format[b];
		const vertex_buffer::config& r_buff = other.compact_vertex_format[b];

		if ( auto cmp = l_buff.stride - r_buff.stride )
		{
			return cmp;
		}

		size_t compare_attrib_count =
			std::min( l_buff.attributes.size(), r_buff.attributes.size() );

		for ( size_t a = 0; a < compare_attrib_count; ++a )
		{
			const vertex_buffer::attribute& l_attr = l_buff.attributes[a];
			const vertex_buffer::attribute& r_attr = r_buff.attributes[a];

			if ( auto cmp = l_attr.offset - r_attr.offset )
			{
				return cmp;
			}
		}

		if ( auto cmp = l_buff.attributes.size() - r_buff.attributes.size() )
		{
			return cmp;
		}
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

	if ( auto cmp = compact_constants.size() - other.compact_constants.size() )
	{
		return cmp;
	}

	return compact_vertex_format.size() - other.compact_vertex_format.size();
}
