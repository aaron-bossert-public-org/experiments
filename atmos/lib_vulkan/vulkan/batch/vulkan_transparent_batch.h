
#pragma once

#include "vulkan/batch/vulkan_batch_nodes.h"
#include "vulkan/defines/vulkan_includes.h"

#include "igpu/batch/transparent_batch.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_pipeline_cache;

	class vulkan_transparent_batch : public transparent_batch
	{
	public:
		struct config : transparent_batch::config
		{
			vulkan_root_batch::vulkan vk;
		};

		const config& cfg() const override;

		~vulkan_transparent_batch() override;

		void render( const utility::frustum& frustum ) override;

		std::unique_ptr< batch_binding > make_binding(
			const instance_batch::config& ) override;

		static std::unique_ptr< vulkan_transparent_batch > make(
			const config& );

	private:
		vulkan_transparent_batch(
			const config&,
			std::unique_ptr< vulkan_root_batch > );

	private:
		const config _cfg;
		std::unique_ptr< vulkan_root_batch > _root_batch;
	};
}
