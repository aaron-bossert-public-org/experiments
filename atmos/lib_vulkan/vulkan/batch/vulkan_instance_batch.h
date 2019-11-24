
#pragma once

#include "igpu/batch/batch_binding.h"
#include "igpu/batch/instance_batch.h"

namespace igpu
{
	struct vulkan_batch_raster_state;
	class vulkan_job_primitives;
	class vulkan_job_buffers;
	class vulkan_program;
	class vulkan_primitives;
	class vulkan_root_batch;

	class vulkan_instance_batch : public instance_batch
	{
	public:
		struct config : instance_batch::config
		{
			struct vulkan
			{
				vulkan_root_batch* root_batch = nullptr;
				std::shared_ptr< vulkan_program > program;
				std::shared_ptr< vulkan_primitives > instance;
			};

			vulkan vk;
		};

		vulkan_instance_batch( const config& cfg );

		void enabled( bool ) override;

		bool enabled() const override;

		void draw_params( const variant_t& ) override;

		const variant_t& draw_params() const override;

		bool can_raster( vulkan_batch_raster_state* );

		void rasterize( const vulkan_batch_raster_state& );

		vulkan_instance_batch() = default;
		vulkan_instance_batch( vulkan_instance_batch&& ) = default;
		vulkan_instance_batch& operator=( vulkan_instance_batch&& ) = default;

	private:
		bool _enabled = true;
		vulkan_root_batch* _root_batch = nullptr;
		variant_t _draw_params;
		std::shared_ptr< vulkan_job_primitives > _job_primitives;
		std::shared_ptr< vulkan_job_buffers > _indirect_draw_dependency;
	};
}
