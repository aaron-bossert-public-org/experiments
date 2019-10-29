
#pragma once

#include <igpu/batch/instance_batch.h>

#include <igpu/batch/batch_binding.h>
#include <igpu/material/material.h>

#include <vulkan/buffer/vulkan_geometry.h>
#include <vulkan/material/vulkan_program.h>
#include <vulkan/material/vulkan_render_states.h>

#include <vulkan/batch/vulkan_batch_nodes.h>

namespace igpu
{
	class vulkan_instance_batch : public instance_batch
	{
	public:

		struct config : instance_batch::config
		{
			struct
			{
				std::shared_ptr<vulkan_program> program;
				std::shared_ptr<vulkan_render_states> render_states;
				std::shared_ptr<vulkan_geometry> geometry;
			} vulkan;
		};

		vulkan_instance_batch(
			const config& cfg,
			const utility::sphere&);

		vulkan_instance_batch() = default;
		vulkan_instance_batch(vulkan_instance_batch&&) = default;
		vulkan_instance_batch& operator= (vulkan_instance_batch&&) = default;

		const config& cfg() const override;

		void instance_count(size_t) override;

		size_t instance_count() const override;

		const utility::sphere& visibility_sphere() const override;

		void visibility_sphere(const utility::sphere& visibility_sphere) override;

	private:

		config _cfg;

		size_t _instance_count = 1;
		utility::sphere _visibility_sphere;
	};
}
