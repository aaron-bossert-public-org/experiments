
#pragma once

#include <memory>
#include <igpu/batch/batch_binding.h>
#include <igpu/utility/utility_types.h>
#include <igpu/batch/batch_binding.h>

#include <functional>
#include <memory>

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
			const instance_batch::config&,
			const utility::sphere& visibility_sphere) = 0;
        
        virtual void render(const utility::frustum& frustum) = 0;

		void visit(const std::function<void(const instance_batch&)>& visit);
        
		
    protected:

		virtual ~batch() = default;
		batch() = default;
		batch(const batch&) = delete;
		batch& operator= (const batch&) = delete;
	};    
}
