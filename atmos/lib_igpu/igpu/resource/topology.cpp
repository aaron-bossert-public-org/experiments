
#include <igpu/resource/topology.h>

#include <framework/logging/log.h>

using namespace igpu;
size_t polycount(topology topology, const size_t index_count)
{
    switch (topology)
    {
	case topology::POINT_LIST:
            return index_count * 2;
        case topology::LINE_LIST:
            return 0;
        case topology::LINE_STRIP:
            return 0;
        case topology::TRIANGLE_LIST:
            return index_count / 3;
        case topology::TRIANGLE_STRIP:
            return index_count - 2;
    }
    
    LOG_CONTEXT( CRITICAL, "unhandled topology(%d)", (int)topology );
    
    return 0;
}
