
#pragma once

#include "gfx/texture/packed_texture.h"

#include <unordered_map>

namespace gfx
{
    class packing_request;
    class texture_packer;
    
    class texture_packers
    {
    public:
        
		using packers_t = std::unordered_map<igpu::texture_format, std::unique_ptr<texture_packer>>;
        using requests_t = std::list<packing_request>;
        
        void process_requests(requests_t& requests);
        
        ~texture_packers();
        
    public:
        
        static std::unique_ptr<texture_packers> make();
        
    private:
        
        texture_packers(packers_t&& packers);
        
        packers_t _packers;
    };
}