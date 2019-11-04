
#include "gfx/perf/mesh/reduced_polycount_geometry.h"

using namespace gfx;

reduced_polycount_geometry::reduced_polycount_geometry(float poly_count_scale, geometry_t geometry)
: geometry(geometry->name(), geometry->group(), geometry->vertex_format())
, _poly_count_scale(poly_count_scale)
, _geometry(geometry)
{
    ASSERT_CONTEXT(poly_count_scale <= 1.f);
    ASSERT_CONTEXT(poly_count_scale >= 0.f);
}

const reduced_polycount_geometry::geometry_t& reduced_polycount_geometry::geometry()
{
    return _geometry;
}

void reduced_polycount_geometry::begin_batched()
{
    _geometry->begin_batched();
}

void reduced_polycount_geometry::end_batched()
{
    _geometry->end_batched();
}

igpu::topology reduced_polycount_geometry::topology() const
{
    return _geometry->topology();
}

size_t reduced_polycount_geometry::element_count() const
{
    // this will result in calling "make_draw_call" function
    int element_count = _geometry->element_count();
    
    if(3 < element_count)
    {
		element_count = (element_count * _poly_count_scale);
        
        // round down to triangle
		element_count /= 3;
		element_count *= 3;
        
        // at least one triangle
		element_count = std::max(element_count, 3);
    }
    
    return element_count;
}

void reduced_polycount_geometry::on_draw_batched()
{
    _context.draw(_geometry->topology(), element_count());
}