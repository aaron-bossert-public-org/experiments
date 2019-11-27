
#pragma once

#include "vulkan/shader/vulkan_constant_parameters.h"
#include "vulkan/shader/vulkan_parameters.h"
#include "vulkan/shader/vulkan_vertex_parameters.h"

#include "igpu/shader/program.h"

#include <array>

namespace igpu
{
	class batch;
	class raster_binding;
	class vulkan_context;
	class vulkan_poset_fence;
	class vulkan_fragment_shader;
	class vulkan_vertex_shader;

	class vulkan_program : public program
	{
	public:
		struct config : program::config
		{
			struct vulkan
			{
				VkDevice device;
				std::shared_ptr< vulkan_vertex_shader > vertex;
				std::shared_ptr< vulkan_fragment_shader > fragment;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		const vulkan_vertex_parameters& vertex_parameters() const override;

		virtual const vulkan_constant_parameters& constant_parameters()
			const = 0;

		virtual const vulkan_parameters& batch_parameters() const = 0;

		virtual const vulkan_parameters& material_parameters() const = 0;

		virtual const vulkan_parameters& instance_parameters() const = 0;

		virtual VkPipelineLayout pipeline_layout() const = 0;

		static std::unique_ptr< vulkan_program > make( const config& );

		virtual ~vulkan_program();

	protected:
		vulkan_program(
			const config&,
			std::vector< vulkan_vertex_parameter >&& );

	private:
		const config _cfg;
		vulkan_vertex_parameters _vertex_parameters;
	};
}
