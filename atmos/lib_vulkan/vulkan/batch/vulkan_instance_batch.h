
#pragma once

#include <igpu/batch/instance_batch.h>

#include <igpu/batch/batch_binding.h>

#include <vulkan/buffer/vulkan_geometry.h>
#include <vulkan/shader/vulkan_primitives.h>
#include <vulkan/shader/vulkan_program.h>
#include <vulkan/shader/vulkan_render_states.h>

namespace igpu
{
	struct vulkan_batch_stack;
	class vulkan_root_batch;

	class vulkan_instance_batch : public instance_batch
	{
	public:

		struct config : instance_batch::config
		{
			struct vulkan
			{
				vulkan_root_batch* root_batch;
				std::shared_ptr<vulkan_program> program;
				std::shared_ptr<vulkan_render_states> render_states;
				std::shared_ptr<vulkan_primitives> material;
				std::shared_ptr<vulkan_geometry> geometry;
				std::shared_ptr<vulkan_primitives> primitives;
			};
			
			vulkan vk;
		};

		vulkan_instance_batch(
			const config& cfg,
			const utility::sphere&);

		void element_start(const std::optional < size_t >&) override;

		const std::optional < size_t >& element_start() const override;

		void element_count(const std::optional < size_t >&) override;

		const std::optional < size_t >& element_count() const override;

		void instance_count(const std::optional < size_t >&) override;

		const std::optional < size_t >& instance_count() const override;

		const utility::sphere& visibility_sphere() const override;

		void visibility_sphere(const utility::sphere& visibility_sphere) override;

		void draw(const vulkan_batch_stack&);

		vulkan_instance_batch() = default;
		vulkan_instance_batch(vulkan_instance_batch&&) = default;
		vulkan_instance_batch& operator= (vulkan_instance_batch&&) = default;

	private:

		std::optional< size_t > _element_start;
		std::optional< size_t > _element_count;
		std::optional< size_t > _instance_count;
		utility::sphere _visibility_sphere;
	};
}
