
#pragma once

#include <igpu/texture/texture_2d.h>

namespace igpu
{
    class texture_2d;
}

namespace gfx
{   
    class packed_texture;

    class packing_request
    {
    public:
		using raw_t = const igpu::texel_buffer_2d;
        using packed_t = std::shared_ptr<packed_texture>;
        
        const raw_t& raw() const;
        
        const packed_t& packed() const;
        
        packing_request(const raw_t&,
                       const packed_t&);
        
    private:
		raw_t& _raw;
        packed_t _packed;
    };
    
    class packed_texture
    {
        
    public:

        const std::shared_ptr<igpu::texture_2d>& atlas() const;
        
        const glm::vec2& uv_scale() const;
        
        const glm::vec2& uv_offset() const;
        
        static std::unique_ptr<packed_texture> make();
        
    public:
        
        void process(const std::shared_ptr<igpu::texture_2d>& atlas,
                     const glm::vec2& uv_scale,
                     const glm::vec2& uv_offset);
        
    private:
        
        packed_texture() = default;
        
        std::shared_ptr<igpu::texture_2d> _atlas;
        
        // when applying to uvs, do "uv = uv * scale + offset;"
        glm::vec2 _uv_scale;
        glm::vec2 _uv_offset;
        
        packed_texture (const packed_texture&) = delete;
        packed_texture& operator= (const packed_texture&) = delete;
    };
    
    struct packed_result_info
    {
		const packing_request::raw_t* raw;
        std::string group;
        glm::vec2 uv_scale;
        glm::vec2 uv_offset;
    };
}

