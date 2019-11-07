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
	if ( vertex_parameters.count() > MAX_PARAMETERS )
	{
		LOG_CRITICAL(
			"vertex parameter count (%d) exceeds limit (%d)",
			(int)vertex_parameters.count(),
			(int)MAX_PARAMETERS );
		memset( this, 0, sizeof *this );
		return false;
	}

	bool success = true;
	size_t buff = 0;
	size_t attr = 0;
	uint32_t attribute_count = 0;

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

		_indices[attribute_count].buffer = buff;
		_indices[attribute_count].attribute = attr;
		++attribute_count;
		++attr;	   // increase attr index of where we expect the next attribute
				   // to be

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

const std::array<
	attribute_indexer::attribute_source,
	attribute_indexer::MAX_PARAMETERS >&
	attribute_indexer::indices() const
{
	return _indices;
}
