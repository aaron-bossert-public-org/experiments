
#pragma once

#include "vulkan/batch/vulkan_instance_batch.h"
#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"
#include "vulkan/sync/vulkan_job.h"

#include "igpu/batch/batch.h"
#include "igpu/batch/batch_nodes.h"
#include "igpu/batch/batch_utility.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_batch_binding;
	class vulkan_command_buffer;
	class vulkan_draw_target;
	class vulkan_fence;
	class vulkan_job_attributes;
	class vulkan_job_primitives;
	class vulkan_pipeline_cache;
	class vulkan_graphics_pipeline;

	class vulkan_root_batch;
	class vulkan_program_batch;
	class vulkan_states_batch;
	class vulkan_geometry_batch;
	class vulkan_material_batch;

	struct vulkan_batch_raster_state : batch::raster_state
	{
		scoped_ptr< vulkan_command_buffer > command_buffer;

		struct
		{
			vulkan_root_batch* root = nullptr;
			vulkan_program_batch* program = nullptr;
			vulkan_states_batch* states = nullptr;
			vulkan_geometry_batch* geometry = nullptr;
			vulkan_material_batch* material = nullptr;
			vulkan_instance_batch* instance = nullptr;
		} batches;

		struct
		{
			int32_t base_vertex = 0;
			uint32_t instance_start = 0;
			uint32_t instance_count = 0;
			uint32_t element_start = 0;
			uint32_t element_count = 0;
		} fallback, resolved;
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
			vulkan_root_batch* root_batch = nullptr;
			std::shared_ptr< vulkan_program > program;
			std::shared_ptr< vulkan_primitives > primitives;
		};

		vulkan_material_batch( const config& );
		~vulkan_material_batch();
		vulkan_material_batch( vulkan_material_batch&& ) = default;
		vulkan_material_batch& operator=( vulkan_material_batch&& ) = default;

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
			vulkan_root_batch* root_batch = nullptr;
			std::shared_ptr< vulkan_program > program;
			std::shared_ptr< vulkan_primitives > batch_primitives;
			std::shared_ptr< vulkan_graphics_pipeline > pipeline;
			std::shared_ptr< vulkan_geometry > geometry;
			std::vector< size_t > active_vertex_buffers;
		};

		vulkan_geometry_batch( const config& );
		~vulkan_geometry_batch();
		vulkan_geometry_batch( vulkan_geometry_batch&& ) = default;
		vulkan_geometry_batch& operator=( vulkan_geometry_batch&& ) = default;

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
			vulkan_root_batch* root_batch = nullptr;
			std::shared_ptr< vulkan_render_states > states;
		};

		vulkan_states_batch( const config& );
		~vulkan_states_batch();
		vulkan_states_batch( vulkan_states_batch&& ) = default;
		vulkan_states_batch& operator=( vulkan_states_batch&& ) = default;

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
			vulkan_root_batch* root_batch = nullptr;
			std::shared_ptr< vulkan_program > program;
		};

		vulkan_program_batch( const config& );
		~vulkan_program_batch();
		vulkan_program_batch( vulkan_program_batch&& ) = default;
		vulkan_program_batch& operator=( vulkan_program_batch&& ) = default;

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

	private:
		config _cfg;
	};

	class vulkan_root_batch
		: public batch_utility::batch_impl_t<
			  root_batch,
			  vulkan_primitives,
			  vulkan_program_batch >
		, public vulkan_job
	{
	public:
		using raster_state_t = vulkan_batch_raster_state;

		struct vulkan
		{
			VkDevice device = nullptr;
			vulkan_context* context = nullptr;
			size_t swap_count = 0;
			scoped_ptr< vulkan_draw_target > draw_target;
			scoped_ptr< vulkan_pipeline_cache > pipeline_cache;
			std::shared_ptr< vulkan_primitives > primitives;
		};

		const vulkan& vk() const;

		void start_raster( const vulkan_batch_raster_state& );

		void stop_raster();

		void rebind_draw_target(
			const scoped_ptr< vulkan_draw_target >& draw_target );

		std::unique_ptr< vulkan_batch_binding > make_binding(
			const instance_batch::config& );

		static std::unique_ptr< vulkan_root_batch > make( const vulkan& );

		~vulkan_root_batch();

	private:
		vulkan_root_batch( const vulkan& );

		vulkan_job::state& job_state() override;

		const vulkan_job::state& job_state() const override;

	private:
		vulkan _vk;
		vulkan_job::state _job_state;
	};

	class vulkan_batch_binding
		: public batch_utility::batch_binding_t< vulkan_root_batch >
	{
	public:
		using batch_utility::batch_binding_t<
			vulkan_root_batch >::batch_binding_t;
		~vulkan_batch_binding();
	};
}
