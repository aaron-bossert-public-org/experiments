
#include "igpu/buffer/geometry.h"

#include "igpu/buffer/vertex_buffer.h"

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
		if ( attributes->at( attr ).parameter.name == name )
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
