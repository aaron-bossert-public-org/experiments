
#include "igpu/buffer/geometry.h"

#include "igpu/buffer/index_buffer.h"
#include "igpu/buffer/vertex_buffer.h"

#include "framework/utility/hash_utils.h"

#include <algorithm>

using namespace igpu;

bool geometry::find_expected_vertex_param(
	const std::string_view& name,
	size_t* p_expected_buff,
	size_t* p_expected_attr ) const
{
	const config& cfg = this->cfg();
	const size_t count_buff = cfg.vertex_buffers.size();
	*p_expected_buff %= count_buff;

	const auto* attributes =
		&cfg.vertex_buffers[*p_expected_buff]->cfg().attributes;
	size_t count_attr = attributes->size();

	if ( *p_expected_attr >= count_attr )
	{
		*p_expected_attr = 0;
		++*p_expected_buff;
		*p_expected_buff %= count_buff;
		attributes = &cfg.vertex_buffers[*p_expected_buff]->cfg().attributes;
	}

	size_t buff = *p_expected_buff;
	size_t attr = *p_expected_attr;

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
			buff %= count_buff;
			attributes = &cfg.vertex_buffers[buff]->cfg().attributes;
		}

	} while ( buff != *p_expected_buff || attr != *p_expected_attr );

	return false;
}

size_t geometry::config::hash() const
{
	size_t h =
		hash_utils::hash_combine( topology, index_buffer, ibuff_byte_offset );

	hash_utils::hash_combine( &h, constants.hash() );

	for ( size_t i = 0; i < vertex_buffers.size(); ++i )
	{
		hash_utils::hash_combine( &h, vertex_buffers[i] );
		if ( i < vbuff_byte_offsets.size() )
		{
			hash_utils::hash_combine( &h, vbuff_byte_offsets[i] );
		}
	}

	return h;
}

ptrdiff_t geometry::config::compare( const config& other ) const
{
	if ( auto cmp = (ptrdiff_t)topology - (ptrdiff_t)other.topology )
	{
		return cmp;
	}

	if ( auto cmp = constants.compare( other.constants ) )
	{
		return cmp;
	}

	if ( auto cmp = index_buffer.get() - other.index_buffer.get() )
	{
		return cmp;
	}

	if ( auto cmp = ibuff_byte_offset - other.ibuff_byte_offset )
	{
		return cmp;
	}

	size_t buff_count =
		std::min( vertex_buffers.size(), other.vertex_buffers.size() );

	for ( size_t i = 0; i < buff_count; ++i )
	{
		if ( auto cmp =
				 vertex_buffers[i].get() - other.vertex_buffers[i].get() )
		{
			return cmp;
		}
	}

	size_t offset_count =
		std::min( vbuff_byte_offsets.size(), other.vbuff_byte_offsets.size() );
	for ( size_t i = 0; i < offset_count; ++i )
	{
		if ( auto cmp = vbuff_byte_offsets[i] - other.vbuff_byte_offsets[i] )
		{
			return cmp;
		}
	}

	if ( auto cmp = vertex_buffers.size() - other.vertex_buffers.size() )
	{
		return cmp;
	}

	return vbuff_byte_offsets.size() - other.vbuff_byte_offsets.size();
}
