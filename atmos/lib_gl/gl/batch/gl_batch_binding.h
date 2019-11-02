
#pragma once

#include <igpu/batch/batch_binding.h>

namespace igpu
{
	class gl_context;

	class gl_batch_binding : public batch_binding
	{
		struct config : instance_batch::config
		{
			struct gl
			{
				gl_context* context = nullptr;
			};

			gl gl;
		};

		const config& cfg() const override;

		const utility::sphere& visibility_sphere() const override;

		const igpu::primitives& primitives() const override;

		void visibility_sphere(const utility::sphere& visibility_sphere) override;

	protected:

		gl_batch_binding(
			const instance_batch::config&,
			const utility::sphere&);

	private:

		const igpu::primitives _primitives;
		utility::sphere _visibility_sphere;
	};

	template < typename BATCH_T >
	class gl_batch_binding_t : public gl_batch_binding
	{
    public:

		struct config
		{
			BATCH_T* batch = nullptr;
		};

		const config& cfg() const
		{
			return _cfg;
		}

		void unbind() override
		{
			_cfg.batch->remove(*this);
			_cfg.batch = nullptr;
		}

		~gl_batch_binding_t() override
		{

		}

		std::unique_ptr<gl_batch_binding_t> make(
			const config::config&,
			const utility::sphere& visibility_sphere) override
		{

		}

	protected:
		gl_batch_binding_t(
			const config::config&,
			const utility::sphere& visibility_sphere);

	private:

		const config _cfg;
	};  
}
