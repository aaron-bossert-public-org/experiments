
#pragma once

#include <igpu/batch/batch_nodes.h>
#include <igpu/batch/batch_utility.h>

#include<vulkan/batch/vulkan_instance_batch.h>

namespace igpu
{
	class vulkan_geometry_batch;
	class vulkan_material_batch;
	class vulkan_render_state_batch;
	class vulkan_program_batch;
	class vulkan_root_batch;

	struct vulkan_batch_stack
	{
		vulkan_root_batch* root_batch = nullptr;
		vulkan_program_batch* program_batch = nullptr;
		vulkan_render_state_batch* render_state_batch = nullptr;
		vulkan_material_batch* material_batch = nullptr;
		vulkan_geometry_batch* geometry_batch = nullptr;
		vulkan_instance_batch* instance_batch = nullptr;
	};

	class vulkan_geometry_batch : public batch_utility::batch_impl_t<geometry_batch, vulkan_instance_batch, vulkan_geometry>
	{
	public:
		vulkan_geometry_batch(const vulkan_instance_batch::config&);

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_geometry* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_material_batch : public batch_utility::batch_impl_t<material_batch, vulkan_geometry_batch, material>
	{
	public:
		vulkan_material_batch(const vulkan_instance_batch::config&);

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static material* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_render_state_batch : public batch_utility::batch_impl_t<render_state_batch, vulkan_material_batch, vulkan_render_states>
	{
	public:
		vulkan_render_state_batch(const vulkan_instance_batch::config&);

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_render_states* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_program_batch : public batch_utility::batch_impl_t<program_batch, vulkan_render_state_batch, vulkan_program>
	{
	public:
		vulkan_program_batch(const vulkan_instance_batch::config&);

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		static vulkan_program* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_root_batch : public batch_utility::batch_impl_t<root_batch, vulkan_program_batch, batch_parameters>
	{
	public:

		using batch_stack_t = vulkan_batch_stack;

		struct config
		{
			vulkan_context* context = nullptr;
		};

		const config& cfg() const;

		void start_draw(const vulkan_batch_stack&);

		void stop_draw();

		std::shared_ptr<batch_binding> make_binding(
			const instance_batch::config&,
			const utility::sphere& visibility_sphere);

		static std::unique_ptr<vulkan_root_batch> make(
			const config&);
		
	private:
		
		vulkan_root_batch(const config& cfg);

	private:

		const config _cfg;
	};
}
