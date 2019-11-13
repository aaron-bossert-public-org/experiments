#include "igpu/shader/attribute_indexer.h"

#include "igpu/buffer/geometry.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/shader/vertex_parameter.h"
#include "igpu/shader/vertex_parameters.h"

#include "framework/logging/log.h"

using namespace igpu;

bool attribute_indexer::reset(
	const vertex_parameters& vertex_parameters,
	const geometry& geometry )
{
	static_assert(
		1 << attribute_source::BITS >= vertex_parameters::MAX_COUNT );

	if ( vertex_parameters.count() > vertex_parameters::MAX_COUNT )
	{
		LOG_CRITICAL(
			"vertex parameter count (%d) exceeds limit (%d)",
			(int)vertex_parameters.count(),
			(int)vertex_parameters::MAX_COUNT );
		memset( this, 0, sizeof *this );
		return false;
	}

	bool success = true;
	size_t buff = 0;
	size_t attr = 0;
	uint32_t attribute_count = 0;
	_buffer_count = 0;

	std::array< uint8_t*, vertex_parameters::MAX_COUNT > buffer_table = {};

	for ( size_t pram = 0; pram < vertex_parameters.count(); ++pram )
	{
		const igpu::vertex_parameter& shader_param =
			vertex_parameters.parameter( pram );

		// buff and attr should contain the indices we expect of the attribute
		// usually the order of attributes in shaders is the same as the order
		// of attributes in vertex buffers, making this exceptionally fast
		if ( !geometry.find_expected_vertex_param(
				 shader_param.cfg().name,
				 &buff,
				 &attr ) )
		{
			LOG_CRITICAL(
				"attribute named (%s) not found in geometry "
				"(%s) ",
				shader_param.cfg().name.c_str(),
				geometry.cfg().name.c_str() );
			success = false;
			continue;
		}

		const vertex_buffer& vertex_buffer = geometry.vertex_buffer( buff );
		const vertex_buffer::attribute& attribute =
			vertex_buffer.cfg().attributes[attr];

		if ( !buffer_table[buff] )
		{
			_buffer_indices[_buffer_count] = (uint8_t)buff;
			buffer_table[buff] = &_buffer_indices[_buffer_count];
			_buffer_count++;
		}

		_indices[attribute_count].buffer = *buffer_table[buff];
		_indices[attribute_count].attribute = attr;

		++attribute_count;

		// increase attr index of where we expect the next attribute to be
		++attr;

		if ( shader_param.cfg().components != attribute.parameter.components )
		{
			LOG_CRITICAL(
				"attribute (%s) expected to be (%s) but geometry (%s) uses "
				"(%s) here instead",
				shader_param.cfg().name.c_str(),
				to_string( shader_param.cfg().components ).data(),
				geometry.cfg().name.c_str(),
				to_string( attribute.parameter.components ).data() );
			success = false;
		}
	}

	return success;
}

const size_t attribute_indexer::buffer_count() const
{
	return _buffer_count;
}

const attribute_indexer::buffer_indices_t& attribute_indexer::buffer_indices()
	const
{
	return _buffer_indices;
}

const attribute_indexer::indices_t& attribute_indexer::indices() const
{
	return _indices;
}
