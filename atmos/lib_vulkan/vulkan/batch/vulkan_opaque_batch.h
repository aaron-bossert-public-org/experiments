
#pragma once

#include "vulkan/batch/vulkan_batch_nodes.h"
#include "vulkan/defines/vulkan_includes.h"

#include "igpu/batch/opaque_batch.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_raster_batch;
	class vulkan_pipeline_cache;

	class vulkan_opaque_batch : public opaque_batch
	{
	public:
		struct config : opaque_batch::config
		{
			vulkan_raster_batch::vulkan vk;
		};

		const opaque_batch::config& cfg() const override;

		~vulkan_opaque_batch() override;

		void raster( const scoped_ptr< draw_target >&, const raster_state& )
			override;

		std::unique_ptr< batch_binding > make_binding(
			const instance_batch::config& ) override;

		static std::unique_ptr< vulkan_opaque_batch > make( const config& );

	private:
		vulkan_opaque_batch(
			const opaque_batch::config&,
			std::unique_ptr< vulkan_raster_batch > );

	private:
		opaque_batch::config _cfg;
		std::unique_ptr< vulkan_raster_batch > _raster_batch;
	};
}
