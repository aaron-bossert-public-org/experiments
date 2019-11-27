
#pragma once

#include "vulkan/batch/vulkan_instance_batch.h"
#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"
#include "vulkan/sync/vulkan_job.h"
#include "vulkan/sync/vulkan_poset_fence.h"

#include "igpu/batch/batch.h"
#include "igpu/batch/batch_nodes.h"
#include "igpu/batch/batch_utility.h"
#include "igpu/shader/constants.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_batch_binding;
	class vulkan_command_buffer;
	class vulkan_draw_target;
	class vulkan_job_attributes;
	class vulkan_job_primitives;
	class vulkan_managers;
	class vulkan_pipeline_cache;
	class vulkan_graphics_pipeline;

	class vulkan_raster_batch;
	class vulkan_program_batch;
	class vulkan_states_batch;
	class vulkan_geometry_batch;
	class vulkan_material_batch;

	struct vulkan_batch_raster_state : batch::raster_state
	{
		scoped_ptr< vulkan_command_buffer > command_buffer;
		vulkan_poset_fence fence;

		struct
		{
			vulkan_raster_batch* root = nullptr;
			vulkan_program_batch* program = nullptr;
			vulkan_states_batch* states = nullptr;
			vulkan_geometry_batch* geometry = nullptr;
			vulkan_material_batch* material = nullptr;
			vulkan_instance_batch* instance = nullptr;
		} batches;

		draw_parameters fallback;
	};

	class vulkan_material_batch
		: public batch_utility::batch_impl_t<
			  material_batch,
			  vulkan_primitives,
			  vulkan_instance_batch >
	{
	public:
		struct config
		{
			vulkan_raster_batch* raster_batch = nullptr;
			std::shared_ptr< vulkan_program > program;
			std::shared_ptr< vulkan_primitives > primitives;
		};

		vulkan_material_batch( const config& );
		~vulkan_material_batch();
		vulkan_material_batch( vulkan_material_batch&& ) = default;
		vulkan_material_batch& operator=( vulkan_material_batch&& ) = default;

		vulkan_primitives& item() const override;

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

	private:
		config _cfg;
		std::shared_ptr< vulkan_job_primitives > _job_primitives;
	};

	class vulkan_geometry_batch
		: public batch_utility::batch_impl_t<
			  geometry_batch,
			  vulkan_geometry,
			  vulkan_material_batch >
	{
	public:
		struct config
		{
			vulkan_raster_batch* raster_batch = nullptr;
			std::shared_ptr< vulkan_program > program;
			std::shared_ptr< vulkan_primitives > batch_primitives;
			std::shared_ptr< vulkan_geometry > geometry;
			std::vector< size_t > active_vertex_buffers;
			std::shared_ptr< vulkan_graphics_pipeline > pipeline;
		};

		vulkan_geometry_batch( const config& );
		~vulkan_geometry_batch();
		vulkan_geometry_batch( vulkan_geometry_batch&& ) = default;
		vulkan_geometry_batch& operator=( vulkan_geometry_batch&& ) = default;

		vulkan_geometry& item() const override;

		[[nodiscard]] bool pre_raster( vulkan_batch_raster_state* );

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

		void rebind_draw_target(
			const scoped_ptr< vulkan_draw_target >& ) const;

	private:
		config _cfg;
		std::shared_ptr< vulkan_job_attributes > _job_attributes;
		std::shared_ptr< vulkan_job_primitives > _job_primitives;
	};

	class vulkan_states_batch
		: public batch_utility::batch_impl_t<
			  states_batch,
			  vulkan_render_states,
			  vulkan_geometry_batch >
	{
	public:
		struct config
		{
			vulkan_raster_batch* raster_batch = nullptr;
			std::shared_ptr< vulkan_render_states > states;
		};

		vulkan_states_batch( const config& );
		~vulkan_states_batch();
		vulkan_states_batch( vulkan_states_batch&& ) = default;
		vulkan_states_batch& operator=( vulkan_states_batch&& ) = default;

		vulkan_render_states& item() const override;

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

	private:
		config _cfg;
	};

	class vulkan_program_batch
		: public batch_utility::
			  batch_impl_t< program_batch, vulkan_program, vulkan_states_batch >
	{
	public:
		struct config
		{
			vulkan_raster_batch* raster_batch = nullptr;
			std::shared_ptr< vulkan_program > program;
		};

		vulkan_program_batch( const config& );
		~vulkan_program_batch();
		vulkan_program_batch( vulkan_program_batch&& ) = default;
		vulkan_program_batch& operator=( vulkan_program_batch&& ) = default;

		vulkan_program& item() const override;

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

	private:
		config _cfg;
	};

	class vulkan_raster_batch
		: public batch_utility::batch_impl_t<
			  raster_batch,
			  vulkan_primitives,
			  vulkan_program_batch >
		, public vulkan_job
	{
	public:
		using raster_state_t = vulkan_batch_raster_state;

		struct vulkan
		{
			vulkan_context* context = nullptr;
			size_t swap_count = 0;
			scoped_ptr< vulkan_managers > managers;
			scoped_ptr< vulkan_draw_target > draw_target;
			scoped_ptr< vulkan_pipeline_cache > pipeline_cache;
			std::shared_ptr< vulkan_primitives > primitives;
			constants::config constants;
		};

		const vulkan& vk() const;

		vulkan_primitives& item() const override;

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

		void rebind_draw_target(
			const scoped_ptr< vulkan_draw_target >& draw_target );

		std::unique_ptr< vulkan_batch_binding > make_binding(
			const instance_batch::config& );

		static std::unique_ptr< vulkan_raster_batch > make( const vulkan& );

		~vulkan_raster_batch();

	private:
		vulkan_raster_batch( const vulkan& );

		vulkan_job::state& job_state() override;

		const vulkan_job::state& job_state() const override;

	private:
		vulkan _vk;
		vulkan_job::state _job_state;
	};

	class vulkan_batch_binding
		: public batch_utility::batch_binding_t< vulkan_raster_batch >
	{
	public:
		using batch_utility::batch_binding_t<
			vulkan_raster_batch >::batch_binding_t;
		~vulkan_batch_binding();
	};
}
