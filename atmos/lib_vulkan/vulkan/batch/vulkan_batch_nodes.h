
#pragma once

#include <igpu/batch/batch_nodes.h>
#include <igpu/batch/batch_utility.h>

#include<vulkan/batch/vulkan_instance_batch.h>

namespace igpu
{
	class vulkan_geometry_batch;
	class vulkan_fence;
	class vulkan_material_batch;
	class vulkan_render_states_batch;
	class vulkan_program_batch;
	class vulkan_root_batch;
	class vulkan_batch_binding;

	struct vulkan_batch_draw_config : batch_draw_config
	{
		struct vulkan
		{
			std::shared_ptr<vulkan_fence> fence;
		};
		
		vulkan vk;
	};

	struct vulkan_batch_stack
	{
		vulkan_root_batch* root_batch = nullptr;
		vulkan_program_batch* program_batch = nullptr;
		vulkan_render_states_batch* render_states_batch = nullptr;
		vulkan_material_batch* material_batch = nullptr;
		vulkan_geometry_batch* geometry_batch = nullptr;
		vulkan_instance_batch* instance_batch = nullptr;
	};

	class vulkan_geometry_batch : public batch_utility::batch_impl_t<geometry_batch, vulkan_instance_batch, vulkan_geometry>
	{
	public:

		vulkan_geometry_batch(const vulkan_instance_batch::config&);
		~vulkan_geometry_batch();
		vulkan_geometry_batch(vulkan_geometry_batch&&) = default;
		vulkan_geometry_batch& operator= (vulkan_geometry_batch&&) = default;


		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_geometry* get_key(const vulkan_instance_batch::config&);

	private:

		vulkan_root_batch* _root_batch;
	};

	class vulkan_material_batch : public batch_utility::batch_impl_t<material_batch, vulkan_geometry_batch, vulkan_primitives>
	{
	public:

		vulkan_material_batch(const vulkan_instance_batch::config&);
		~vulkan_material_batch();
		vulkan_material_batch(vulkan_material_batch&&) = default;
		vulkan_material_batch& operator= (vulkan_material_batch&&) = default;

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_primitives* get_key(const vulkan_instance_batch::config&);

	private:

		vulkan_root_batch* _root_batch;
	};

	class vulkan_render_states_batch : public batch_utility::batch_impl_t<render_states_batch, vulkan_material_batch, vulkan_render_states>
	{
	public:

		vulkan_render_states_batch(const vulkan_instance_batch::config&);
		~vulkan_render_states_batch();
		vulkan_render_states_batch(vulkan_render_states_batch&&) = default;
		vulkan_render_states_batch& operator= (vulkan_render_states_batch&&) = default;

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_render_states* get_key(const vulkan_instance_batch::config&);

	private:

		vulkan_root_batch* _root_batch;
	};

	class vulkan_program_batch : public batch_utility::batch_impl_t<program_batch, vulkan_render_states_batch, vulkan_program>
	{
	public:

		vulkan_program_batch(const vulkan_instance_batch::config&);
		~vulkan_program_batch();
		vulkan_program_batch(vulkan_program_batch&&) = default;
		vulkan_program_batch& operator= (vulkan_program_batch&&) = default;

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_program* get_key(const vulkan_instance_batch::config&);

	private:

		vulkan_root_batch* _root_batch;
	};

	class vulkan_root_batch : public batch_utility::batch_impl_t<root_batch, vulkan_program_batch, vulkan_primitives>
	{
	public:

		using batch_stack_t = vulkan_batch_stack;

		struct config
		{
			vulkan_context* context = nullptr;
		};

		const config& cfg() const;

		void start_draw(const vulkan_batch_draw_config&);

		void stop_draw();

		const std::shared_ptr<vulkan_fence>& fence() const;

		std::unique_ptr<vulkan_batch_binding> make_binding(
			const instance_batch::config&,
			const utility::sphere& visibility_sphere);

		static std::unique_ptr<vulkan_root_batch> make(
			const config&);

		~vulkan_root_batch();
		
	private:
		
		vulkan_root_batch(const config& cfg);

	private:

		const config _cfg;
		std::shared_ptr<vulkan_fence> _fence;
	};

	class vulkan_batch_binding : public batch_utility::batch_binding_t<vulkan_root_batch>
	{
	public:

		using batch_utility::batch_binding_t<vulkan_root_batch>::batch_binding_t;
		~vulkan_batch_binding();
	};
}
