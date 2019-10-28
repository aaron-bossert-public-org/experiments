
#pragma once

#include <igpu/batch/primitive_block.h>
#include <igpu/utility/utility_types.h>

#include <memory>

namespace igpu
{   
	class geometry;
	class material;
	class primitive_block;
	class primitives;
	class program;

    class batch_binding
    {
    public:
        
        struct config
        {
            std::shared_ptr<program> program;
            std::shared_ptr<material> material;
            std::shared_ptr<geometry> geometry;
			std::vector<primitive> primitives;
        };
        
        virtual ~batch_binding() = 0;
        
        virtual void unbind() = 0;
        
        virtual const config& cfg() const = 0;
        
        virtual const utility::sphere& visibility_sphere() const = 0;
        
		virtual const igpu::primitive_block& primitive_block() const = 0;
        
        virtual void visibility_sphere(const utility::sphere& visibility_sphere) = 0;

    protected:

		batch_binding() = default;

		batch_binding(const batch_binding&) = delete;
		batch_binding& operator= (const batch_binding&) = delete;
    };
}

