
#pragma once
#include <memory>
#include <igpu/batch/batch_binding.h>

namespace igpu
{
	class batch_parameters;

	class batch
	{
    public:

        virtual std::shared_ptr<batch_binding> make_binding(
			const batch_binding::config&,
			const utility::sphere& visibility_sphere) = 0;
        
        virtual void render(const utility::frustum& frustum) = 0;
        
		virtual bool empty() const = 0;

        virtual ~batch() = 0;
		
		batch_parameters& parameters();

		const batch_parameters& parameters() const;

    protected:
        
		batch(std::shared_ptr <batch_parameters>);
		batch(const batch&) = default;
		batch& operator= (const batch&) = default;

	private:

		std::shared_ptr <batch_parameters> _parameters;
	};    
}
