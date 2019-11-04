
#pragma once

#include "gfx/perf/context/reduce_batching_system_state_changes.h"

struct perf_test_config 
{
    float view_port_scale = 1.0;
    float polycount_scale = 1.0;    
    perf_test_batching_config batching_config;

    struct replace_texture_config
    {
        bool replace_textures = false;
        int reduction_level_pow2 = 0;
        
    } replace_texture_config;
};
