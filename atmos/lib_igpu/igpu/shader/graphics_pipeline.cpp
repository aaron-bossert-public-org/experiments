
#include "igpu/shader/graphics_pipeline.h"

#include "igpu/buffer/geometry.h"
#include "igpu/shader/attribute_indexer.h"
#include "igpu/shader/program.h"
#include "igpu/shader/render_states.h"
#include "igpu/shader/vertex_parameters.h"

#include "framework/utility/hash_utils.h"

using namespace igpu;

graphics_pipeline::config graphics_pipeline::make_config(
	const attribute_indexer& indexer,
	const std::shared_ptr< program >& program,
	const std::shared_ptr< render_states >& render_states )
{
	graphics_pipeline::config cfg;
	cfg.program = program;
	cfg.render_states = render_states;
	cfg.topology = indexer.geometry()->cfg().topology;

	cfg.compact_vertex_format.resize( indexer.buffer_count() );

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

	return cfg;
}

size_t graphics_pipeline::config::hash( const config& cfg )
{
	size_t h = hash_utils::hash_combine(
		cfg.program,
		cfg.render_states,
		(size_t)cfg.topology );

	for ( const auto& buffer_config : cfg.compact_vertex_format )
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

	return h;
}

ptrdiff_t graphics_pipeline::config::compare(
	const config& lhs,
	const config& rhs )
{
	if ( lhs.program != rhs.program )
	{
		return lhs.program.get() - rhs.program.get();
	}

	if ( lhs.render_states != rhs.render_states )
	{
		return lhs.render_states.get() - rhs.render_states.get();
	}

	if ( lhs.topology != rhs.topology )
	{
		return (ptrdiff_t)lhs.topology - (ptrdiff_t)rhs.topology;
	}

	size_t compare_buffer_count = std::min(
		lhs.compact_vertex_format.size(),
		rhs.compact_vertex_format.size() );

	for ( size_t b = 0; b < compare_buffer_count; ++b )
	{
		const vertex_buffer::config& l_buff = lhs.compact_vertex_format[b];
		const vertex_buffer::config& r_buff = rhs.compact_vertex_format[b];

		if ( l_buff.stride != r_buff.stride )
		{
			return l_buff.stride - r_buff.stride;
		}

		size_t compare_attrib_count =
			std::min( l_buff.attributes.size(), r_buff.attributes.size() );

		for ( size_t a = 0; a < compare_attrib_count; ++a )
		{
			const vertex_buffer::attribute& l_attr = l_buff.attributes[a];
			const vertex_buffer::attribute& r_attr = r_buff.attributes[a];

			if ( l_attr.offset != r_attr.offset )
			{
				return l_attr.offset - r_attr.offset;
			}
		}

		if ( l_buff.attributes.size() != r_buff.attributes.size() )
		{
			return l_buff.attributes.size() - r_buff.attributes.size();
		}
	}

	return lhs.compact_vertex_format.size() - rhs.compact_vertex_format.size();
}
