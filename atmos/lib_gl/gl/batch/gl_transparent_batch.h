
#pragma once

#include <igpu/batch/transparent_batch.h>

#include <vector>
#include <unordered_map>

namespace igpu
{
	class gl_context;

	class gl_transparent_batch : public transparent_batch
	{
	public:
        
        struct binding : batch_binding
        {
			const config& cfg() const override;

            binding(
				gl_transparent_batch*,
				const batch_binding::config&,
				const utility::sphere& visibility_sphere);

            ~binding();

            void unbind() override;
            
		private:

			const config _cfg;
            gl_transparent_batch * _gl_transparent_batch;
        };
        
        using sortable_t = std::pair<float, const binding*>;
        using sorted_t = std::vector<sortable_t>;
        using binding_t = std::shared_ptr<const binding>;
        using bindings_t = std::unordered_map<const binding*, binding_t>;
        
		std::shared_ptr<batch_binding> make_binding(
			const batch_binding::config&,
			const utility::sphere& visibility_sphere) override;

        void render(const utility::frustum&) override;

		bool empty() const override;
        
        ~gl_transparent_batch() override;

        void unbind(const binding*);

		static std::unique_ptr<gl_transparent_batch> make(
			std::shared_ptr<batch_parameters> parameters, 
			gl_context* gl_context);

    private:

		gl_transparent_batch(
			std::shared_ptr<batch_parameters> parameters, 
			gl_context* gl_context);

	private:
        
		gl_context* _gl_context;
        bindings_t _bindings;
        
        // retain set of sortable indices so that they do not need to
        // be re allocated each frame.
        sorted_t _sorted_visible_bindings;
	};
}
