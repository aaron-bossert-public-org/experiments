
#pragma once

#include <igpu/batch/batch_nodes.h>

#include<vulkan/batch/vulkan_instance_batch.h>

namespace igpu
{
	class vulkan_geometry_batch;
	class vulkan_material_batch;
	class vulkan_render_state_batch;
	class vulkan_program_batch;
	class vulkan_root_batch;

	class vulkan_geometry_batch : public batch_utility::batch_impl_t<geometry_batch, vulkan_instance_batch, vulkan_geometry>
	{
	public:
		vulkan_geometry_batch(const vulkan_instance_batch::config&);

		static vulkan_geometry* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_material_batch : public batch_utility::batch_impl_t<material_batch, vulkan_geometry_batch, material>
	{
	public:
		vulkan_material_batch(const vulkan_instance_batch::config&);

		static material* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_render_state_batch : public batch_utility::batch_impl_t<render_state_batch, vulkan_material_batch, vulkan_render_states>
	{
	public:
		vulkan_render_state_batch(const vulkan_instance_batch::config&);

		static vulkan_render_states* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_program_batch : public batch_utility::batch_impl_t<program_batch, vulkan_render_state_batch, vulkan_program>
	{
	public:
		vulkan_program_batch(const vulkan_instance_batch::config&);

		static vulkan_program* get_key(const vulkan_instance_batch::config&);
	};

	class vulkan_root_batch : public batch_utility::batch_impl_t<root_batch, vulkan_program_batch, batch_parameters>
	{
	public:

		struct config
		{
			vulkan_context* context = nullptr;
		};

		vulkan_root_batch(const config& cfg);

		const config& cfg() const;

		std::unique_ptr<batch_binding> make_binding(
			const vulkan_instance_batch::config&,
			const utility::sphere& visibility_sphere);
	private:

		const config& _cfg;
	};
}
