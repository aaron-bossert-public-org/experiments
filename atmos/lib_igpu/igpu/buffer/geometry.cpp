
#include "igpu/buffer/geometry.h"

#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/vertex_buffer.h"

#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

bool geometry::find_expected_vertex_param(
	const std::string& name,
	size_t* p_expected_buff,
	size_t* p_expected_attr ) const
{
	const config& cfg = this->cfg();
	const size_t count_buff = cfg.vertex_buffers.size();

	size_t expected_buff = *p_expected_buff;
	if ( expected_buff >= count_buff )
	{
		expected_buff = 0;
	}

	const auto* attributes =
		&cfg.vertex_buffers[expected_buff]->cfg().attributes;
	size_t count_attr = attributes->size();
	size_t expected_attr = *p_expected_attr;

	if ( expected_attr >= count_attr )
	{
		expected_attr = 0;
	}

	size_t buff = expected_buff;
	size_t attr = expected_attr;


	do
	{
		if ( attributes->at( attr ).name == name )
		{
			*p_expected_buff = buff;
			*p_expected_attr = attr;
			return true;
		}

		++attr;
		if ( attr >= count_attr )
		{
			attr = 0;
			++buff;
			if ( buff >= count_buff )
			{
				buff = 0;
				attributes = &cfg.vertex_buffers[buff]->cfg().attributes;
				count_attr = attributes->size();
			}
		}
	} while ( buff != expected_buff || attr != expected_attr );

	return false;
}

size_t geometry::config::hash( const config& cfg )
{
	size_t h = hash_utils::hash_combine(
		cfg.topology,
		cfg.index_buffer,
		cfg.ibuff_byte_offset );

	for ( size_t i = 0; i < cfg.vertex_buffers.size(); ++i )
	{
		hash_utils::hash_combine( &h, cfg.vertex_buffers[i] );
		if ( i < cfg.vbuff_byte_offsets.size() )
		{
			hash_utils::hash_combine( &h, cfg.vbuff_byte_offsets[i] );
		}
	}

	return h;
}

ptrdiff_t geometry::config::compare( const config& lhs, const config& rhs )
{
	if ( lhs.topology != rhs.topology )
	{
		return (ptrdiff_t)lhs.topology - (ptrdiff_t)rhs.topology;
	}

	if ( lhs.index_buffer != rhs.index_buffer )
	{
		return lhs.index_buffer.get() - rhs.index_buffer.get();
	}

	if ( lhs.ibuff_byte_offset != rhs.ibuff_byte_offset )
	{
		return lhs.ibuff_byte_offset - rhs.ibuff_byte_offset;
	}

	size_t buff_count =
		std::min( lhs.vertex_buffers.size(), rhs.vertex_buffers.size() );

	for ( size_t i = 0; i < buff_count; ++i )
	{
		if ( lhs.vertex_buffers[i] != rhs.vertex_buffers[i] )
		{
			return lhs.vertex_buffers[i].get() - rhs.vertex_buffers[i].get();
		}
	}

	size_t offset_count = std::min(
		lhs.vbuff_byte_offsets.size(),
		rhs.vbuff_byte_offsets.size() );
	for ( size_t i = 0; i < offset_count; ++i )
	{
		if ( lhs.vbuff_byte_offsets[i] != rhs.vbuff_byte_offsets[i] )
		{
			return lhs.vbuff_byte_offsets[i] - rhs.vbuff_byte_offsets[i];
		}
	}

	if ( lhs.vertex_buffers.size() != rhs.vertex_buffers.size() )
	{
		return lhs.vertex_buffers.size() - rhs.vertex_buffers.size();
	}

	return lhs.vbuff_byte_offsets.size() - rhs.vbuff_byte_offsets.size();
}
