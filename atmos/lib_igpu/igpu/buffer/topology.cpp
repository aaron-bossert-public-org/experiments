
#include "igpu/buffer/topology.h"

#include "framework/logging/log.h"

using namespace igpu;

size_t polycount( topology topology, const size_t index_count )
{
	if ( !is_valid( topology ) )
	{
		LOG_CRITICAL( "invalid topology:%d", (int)topology );
	}
	else
	{
		switch ( topology )
		{
		case topology::TRIANGLE_LIST:
			return index_count / 3;
		case topology::TRIANGLE_STRIP:
			return index_count - 2;
		}

		LOG_CRITICAL( "unhandled topology(%s)", to_string( topology ).data() );
	}

	return 0;
}
