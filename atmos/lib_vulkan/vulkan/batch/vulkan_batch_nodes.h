
#pragma once

#include "vulkan/batch/vulkan_instance_batch.h"
#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/shader/vulkan_attributes_decriptor.h"
#include "vulkan/shader/vulkan_graphics_pipeline.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/sync/vulkan_job.h"

#include "igpu/batch/batch_nodes.h"
#include "igpu/batch/batch_utility.h"

namespace igpu
{
	class vulkan_fence;
	class vulkan_job_attributes;
	class vulkan_job_primitives;
	class vulkan_pipeline_cache;

	class vulkan_geometry_batch;
	class vulkan_material_batch;
	class vulkan_graphics_pipeline_batch;
	class vulkan_program_batch;
	class vulkan_root_batch;
	class vulkan_batch_binding;

	struct vulkan_batch_draw_state : batch_draw_state
	{
		std::shared_ptr< vulkan_fence > fence;
		VkCommandBuffer command_buffer;
		struct
		{
			vulkan_root_batch* root = nullptr;
			vulkan_program_batch* program = nullptr;
			vulkan_graphics_pipeline_batch* graphics_pipeline = nullptr;
			vulkan_material_batch* material = nullptr;
			vulkan_geometry_batch* geometry = nullptr;
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

	class vulkan_geometry_batch
		: public batch_utility::batch_impl_t<
			  geometry_batch,
			  vulkan_instance_batch,
			  vulkan_geometry >
	{
	public:
		vulkan_geometry_batch( const vulkan_instance_batch::config& );
		~vulkan_geometry_batch();
		vulkan_geometry_batch( vulkan_geometry_batch&& ) = default;
		vulkan_geometry_batch& operator=( vulkan_geometry_batch&& ) = default;

		[[nodiscard]] bool pre_draw( vulkan_batch_draw_state* );

		void start_draw( const vulkan_batch_draw_state& );

		void stop_draw();

		static vulkan_geometry* get_key( const vulkan_instance_batch::config& );

	private:
		vulkan_root_batch* _root_batch;
		std::shared_ptr< vulkan_job_attributes > _job_attributes;
	};

	class vulkan_material_batch
		: public batch_utility::batch_impl_t<
			  material_batch,
			  vulkan_geometry_batch,
			  vulkan_primitives >
	{
	public:
		vulkan_material_batch( const vulkan_instance_batch::config& );
		~vulkan_material_batch();
		vulkan_material_batch( vulkan_material_batch&& ) = default;
		vulkan_material_batch& operator=( vulkan_material_batch&& ) = default;

		void start_draw( const vulkan_batch_draw_state& );

		void stop_draw();

		static vulkan_primitives* get_key(
			const vulkan_instance_batch::config& );

	private:
		vulkan_root_batch* _root_batch;
		std::shared_ptr< vulkan_job_primitives > _job_primitives;
	};

	class vulkan_graphics_pipeline_batch
		: public batch_utility::batch_impl_t<
			  graphics_pipeline_batch,
			  vulkan_material_batch,
			  vulkan_graphics_pipeline >
	{
	public:
		vulkan_graphics_pipeline_batch( const vulkan_instance_batch::config& );
		~vulkan_graphics_pipeline_batch();
		vulkan_graphics_pipeline_batch( vulkan_graphics_pipeline_batch&& ) =
			default;
		vulkan_graphics_pipeline_batch& operator=(
			vulkan_graphics_pipeline_batch&& ) = default;

		void start_draw( const vulkan_batch_draw_state& );

		void stop_draw();

		static vulkan_graphics_pipeline* get_key(
			const vulkan_instance_batch::config& );

	private:
		vulkan_root_batch* _root_batch;
	};

	class vulkan_program_batch
		: public batch_utility::batch_impl_t<
			  program_batch,
			  vulkan_graphics_pipeline_batch,
			  vulkan_program >
	{
	public:
		vulkan_program_batch( const vulkan_instance_batch::config& );
		~vulkan_program_batch();
		vulkan_program_batch( vulkan_program_batch&& ) = default;
		vulkan_program_batch& operator=( vulkan_program_batch&& ) = default;

		void start_draw( const vulkan_batch_draw_state& );

		void stop_draw();

		static vulkan_program* get_key( const vulkan_instance_batch::config& );

	private:
		vulkan_root_batch* _root_batch;
	};

	class vulkan_root_batch
		: public batch_utility::batch_impl_t<
			  root_batch,
			  vulkan_program_batch,
			  vulkan_primitives >
		, public vulkan_job
	{
	public:
		using draw_state_t = vulkan_batch_draw_state;

		struct vulkan
		{
			VkDevice device = nullptr;
			size_t swap_count = 0;
			scoped_ptr< vulkan_pipeline_cache > pipeline_cache;
		};

		const vulkan& vk() const;

		void start_draw( const vulkan_batch_draw_state& );

		void stop_draw();

		const std::shared_ptr< vulkan_fence >& fence() const;

		std::unique_ptr< vulkan_batch_binding > make_binding(
			const instance_batch::config& );

		static std::unique_ptr< vulkan_root_batch > make( const vulkan& );

		~vulkan_root_batch();

	private:
		vulkan_root_batch( const vulkan& );

	private:
		const vulkan _vk;
		std::shared_ptr< vulkan_fence > _fence;
		std::shared_ptr< vulkan_job_primitives > _job_primitives;
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
