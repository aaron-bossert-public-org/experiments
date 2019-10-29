
#pragma once
#include <memory>
#include <igpu/batch/batch_binding.h>

namespace igpu
{
	class batch_parameters;

	class batch
	{
    public:

		struct config
		{
			std::vector<primitive_kv> primitives;
		};

		virtual const config& cfg() const = 0;

        virtual std::shared_ptr<batch_binding> make_binding(
			const batch_binding::config&,
			const utility::sphere& visibility_sphere) = 0;
        
        virtual void render(const utility::frustum& frustum) = 0;
        
        virtual ~batch() = 0;
		
    protected:
        
		batch() = default;
		batch(const batch&) = delete;
		batch& operator= (const batch&) = delete;
	};    
}
