
#pragma once

#include <vulkan/buffer/vulkan_vertex_parameter.h>
#include <vulkan/material/vulkan_parameter.h>

#include <igpu/material/program.h>
#include <framework/perf/metrics.h>
#include <framework/utility/buffer_view.h>

namespace igpu
{
	class batch;
	class batch_binding;
	class material;
	class vulkan_context;

	class vulkan_program : public program
	{
	public:

		struct config : program::config
		{
			vulkan_context* context;
		};

		const config& cfg() const override;

		~vulkan_program() override;

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

		static std::unique_ptr<vulkan_program> make(
			const config&);

	protected:

		vulkan_program(
			const config&,
			std::vector<vulkan_parameter*> parameters,
			std::vector<vulkan_parameter> batch_parameters,
			std::vector<vulkan_parameter> material_parameters,
			std::vector<vulkan_parameter> instance_parameters,
			std::vector<vulkan_vertex_parameter> vertex_parameters);

	private:

		const config _cfg;
		const std::vector<vulkan_parameter*> _parameters;
		const std::vector<vulkan_parameter> _batch_parameters;
		const std::vector<vulkan_parameter> _material_parameters;
		const std::vector<vulkan_parameter> _instance_parameters;
		const std::vector<vulkan_vertex_parameter> _vertex_parameters;
		perf::metric _texture_switch_metric;
		perf::metric _parameter_switch_metric;
	};
}
