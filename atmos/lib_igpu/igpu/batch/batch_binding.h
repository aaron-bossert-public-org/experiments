
#pragma once

#include <igpu/batch/primitive_block.h>
#include <igpu/utility/utility_types.h>

#include <memory>

namespace igpu
{   
	class geometry;
	class material;

    class batch_binding
    {
    public:
        
        struct config
        {
            std::shared_ptr<material> material;
            std::shared_ptr<geometry> geometry;
        };
        
        virtual ~batch_binding() = 0;
        
        virtual void unbind() = 0;
        
        virtual const config& cfg() const = 0;
        
        const utility::sphere& visibility_sphere() const;
        
        void visibility_sphere(const utility::sphere& visibility_sphere);
        
		const igpu::primitive_block& primitive_block() const;

    protected:
        
        batch_binding(
            const config&,
            const utility::sphere& visibility_sphere);
        
    private:
        
        utility::sphere _visibility_sphere;
		igpu::primitive_block _primitive_block;
    };
}

