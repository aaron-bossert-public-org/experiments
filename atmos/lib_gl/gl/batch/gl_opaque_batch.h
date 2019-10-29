
#pragma once

#include <igpu/batch/opaque_batch.h>

namespace igpu
{
	class gl_context;

	class gl_opaque_batch : opaque_batch
	{
    public:

		~gl_opaque_batch() override;

        void render(const utility::frustum& frustum) override;
        
        std::unique_ptr<batch_binding> make_binding(
			const instance_batch::config&,
			const utility::sphere& visibility_sphere) override;
        
        static std::unique_ptr<gl_opaque_batch> make(
			std::shared_ptr<batch_parameters> parameters,
			gl_context*);

	private:
        
        gl_opaque_batch(
			std::shared_ptr<batch_parameters>,
			std::unique_ptr<opaque_batch_root>,
			gl_context*);

	private:

		gl_context *const _gl_context;
	};    
}
