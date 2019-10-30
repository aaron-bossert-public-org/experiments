
#pragma once

#include <igpu/batch/opaque_batch.h>

namespace igpu
{
	class vulkan_root_batch;
	class vulkan_context;

	class vulkan_opaque_batch : public opaque_batch
	{
	public:
		struct config : opaque_batch::config
		{
			struct {
				vulkan_context* context = nullptr;
			} vulkan;
		};

		const config& cfg() const override;

		~vulkan_opaque_batch() override;

		void render(const utility::frustum& frustum) override;

		std::shared_ptr<batch_binding> make_binding(
			const instance_batch::config&,
			const utility::sphere& visibility_sphere) override;

		static std::unique_ptr<vulkan_opaque_batch> make(
			const config&);

	private:

		vulkan_opaque_batch(
			const config&,
			std::unique_ptr<vulkan_root_batch>);

	private:

		const config _cfg;
		std::unique_ptr<vulkan_root_batch> _root_batch;
	};
}
