
#pragma once

#include "vulkan/buffer/vulkan_vertex_parameter.h"
#include "vulkan/shader/vulkan_parameter.h"

#include "igpu/shader/program.h"
#include "framework/perf/metrics.h"
#include "framework/utility/buffer_view.h"

#include <array>

namespace igpu
{
	class batch;
	class batch_binding;
	class vulkan_context;
	class vulkan_fence;
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
				std::shared_ptr<vulkan_vertex_shader> vertex;
				std::shared_ptr<vulkan_fragment_shader> fragment;
			};

			vulkan vk;
		};

		const config& cfg() const override;

		size_t parameter_count() const override;

		const vulkan_parameter& parameter(size_t) const override;

		size_t vertex_parameter_count() const override;
		const vulkan_vertex_parameter& vertex_parameter(size_t) const override;

		size_t batch_parameter_count() const;
		const vulkan_parameter& batch_parameter(size_t) const;

		size_t material_parameter_count() const;
		const vulkan_parameter& material_parameter(size_t) const;

		size_t instance_parameter_count() const;
		const vulkan_parameter& instance_parameter(size_t) const;

		const std::array<VkDescriptorSetLayout, 3>& descriptor_set_layouts() const;

		VkPipelineLayout pipeline_layout() const;

		static std::unique_ptr<vulkan_program> make(
			const config&);

		~vulkan_program() override;

	protected:

		vulkan_program(
			const config&,
			std::vector<vulkan_parameter> batch_parameters,
			std::vector<vulkan_parameter> material_parameters,
			std::vector<vulkan_parameter> instance_parameters,
			std::vector<vulkan_parameter*>,
			std::vector<vulkan_vertex_parameter>,
			std::array<VkDescriptorSetLayout, 3> descriptor_set_layout,
			VkPipelineLayout);

	private:

		const config _cfg;
		const std::vector<vulkan_parameter> _batch_parameters;
		const std::vector<vulkan_parameter> _material_parameters;
		const std::vector<vulkan_parameter> _instance_parameters;
		const std::vector<vulkan_parameter*> _parameters;
		const std::vector<vulkan_vertex_parameter> _vertex_parameters;
		
		std::array<VkDescriptorSetLayout, 3> _descriptor_set_layouts;
		VkPipelineLayout _pipeline_layout;

		perf::metric _texture_switch_metric;
		perf::metric _parameter_switch_metric;
	};
}
