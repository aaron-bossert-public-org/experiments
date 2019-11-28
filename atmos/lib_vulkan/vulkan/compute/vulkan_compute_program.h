
#pragma once

#include "vulkan/shader/vulkan_constant_parameters.h"
#include "vulkan/shader/vulkan_parameters.h"

#include "igpu/compute/compute_program.h"

#include <array>
#include <memory>

namespace igpu
{
	class batch;
	class raster_binding;
	class vulkan_context;
	class vulkan_poset_fence;
	class vulkan_compute_shader;

	class vulkan_compute_program : public compute_program
	{
	public:
		struct config : compute_program::config
		{
			struct vulkan
			{
				VkDevice device;
				std::shared_ptr< vulkan_compute_shader > compute;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		virtual const vulkan_constant_parameters& constant_parameters()
			const = 0;

		virtual const vulkan_parameters& batch_parameters() const = 0;

		virtual const vulkan_parameters& material_parameters() const = 0;

		virtual const vulkan_parameters& instance_parameters() const = 0;

		virtual VkPipelineLayout pipeline_layout() const = 0;

		static std::unique_ptr< vulkan_compute_program > make( const config& );

		~vulkan_compute_program() override;

	protected:
		vulkan_compute_program( const config& );

	private:
		const config _cfg;
	};
}
