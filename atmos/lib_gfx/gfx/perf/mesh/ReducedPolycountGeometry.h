
#pragma once

#include <gfx/mesh/geometry.h>

namespace gfx
{
    class reduced_polycount_geometry : public geometry
    {
    public:
        
        using geometry_t = std::shared_ptr<geometry>;
        
        reduced_polycount_geometry(float poly_count_scale, geometry_t);
        
        const geometry_t& geometry();
        
    private:
        
        void begin_batched() override;
        
        void end_batched() override;
        
        igpu::topology topology() const override;
        
        size_t element_count() const override;
        
        void on_draw_batched() override;
        
    private:
        
        float _poly_count_scale;
		geometry_t _geometry;
    };
}
