#include "igpu/shader/attribute_finder.h"

#include "igpu/buffer/geometry.h"
#include "igpu/buffer/vertex_buffer.h"
#include "igpu/buffer/vertex_parameter.h"
#include "igpu/shader/program.h"

#include "framework/logging/log.h"

using namespace igpu;

bool attribute_finder::find_all_attributes(
	const program& program,
	const geometry& geometry )
{
	static_assert( sizeof attribute_source == 1 );
	static_assert( sizeof attribute_finder == 16 );

	bool success = true;
	size_t buff = 0;
	size_t attr = 0;
	uint32_t attribute_count = 0;

	for ( size_t pram = 0; pram < program.vertex_parameter_count(); ++pram )
	{
		const igpu::vertex_parameter& shader_param =
			program.vertex_parameter( pram );

		// buff and attr should contain the indices we expect of the attribute
		// usually the order of attributes in shaders is the same as the order
		// of attributes in vertex buffers, making this exceptionally fast
		if ( !geometry.find_expected_vertex_param(
				 shader_param.cfg().name,
				 &buff,
				 &attr ) )
		{
			LOG_CRITICAL(
				"program (%s) "
				"expects geometry (%s) "
				"to contain attribute named (%s) but could not find it",
				program.cfg().name.c_str(),
				geometry.cfg().name.c_str(),
				shader_param.cfg().name.c_str() );
			success = false;
			continue;
		}

		const vertex_buffer& vertex_buffer = geometry.vertex_buffer( buff );
		const vertex_buffer::attribute& attribute =
			vertex_buffer.cfg().attributes[attr];

		_attribute_sources[attribute_count].buffer_index = buff;
		_attribute_sources[attribute_count].attribute_index = attr;
		++attribute_count;
		++attr;	   // increase attr index of where we expect the next attribute
				   // to be

		if ( shader_param.cfg().components != attribute.parameter.components )
		{
			LOG_CRITICAL(
				"program (%s) "
				"expects geometry (%s) "
				"attribute (%s) "
				"to be (%s), but found ",
				"(%s) here instead",
				program.cfg().name.c_str(),
				geometry.cfg().name.c_str(),
				shader_param.cfg().name.c_str(),
				to_string( shader_param.cfg().components ).data(),
				to_string( attribute.parameter.components ).data() );
			success = false;
		}
	}

	return success;
}

const std::array< attribute_finder::attribute_source, 16 >& attribute_finder::
	attribute_sources() const
{
	return _attribute_sources;
}
