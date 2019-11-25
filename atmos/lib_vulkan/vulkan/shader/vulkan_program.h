
#pragma once

#include "vulkan/shader/vulkan_constant_parameters.h"
#include "vulkan/shader/vulkan_parameters.h"
#include "vulkan/shader/vulkan_vertex_parameters.h"

#include "igpu/shader/program.h"

#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"

#include <array>

namespace igpu
{
	class batch;
	class batch_binding;
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

		const vulkan_constant_parameters& constant_parameters() const override;

		const vulkan_vertex_parameters& vertex_parameters() const override;

		const vulkan_parameters& batch_parameters() const override;

		const vulkan_parameters& material_parameters() const override;

		const vulkan_parameters& instance_parameters() const override;

		VkPipelineLayout pipeline_layout() const;

		static std::unique_ptr< vulkan_program > make( const config& );

		~vulkan_program() override;

	protected:
		vulkan_program(
			const config&,
			VkPipelineLayout,
			constant_parameters::config&&,
			std::vector< vulkan_vertex_parameter >&&,
			vulkan_parameters::config&& batch_cfg,
			vulkan_parameters::config&& material_cfg,
			vulkan_parameters::config&& instance_cfg );

	private:
		const config _cfg;

		VkPipelineLayout _pipeline_layout;
		vulkan_constant_parameters _constants;
		vulkan_vertex_parameters _vertex_parameters;
		vulkan_parameters _batch_parameters;
		vulkan_parameters _material_parameters;
		vulkan_parameters _instance_parameters;

		perf::metric _texture_switch_metric;
		perf::metric _parameter_switch_metric;
	};
}
