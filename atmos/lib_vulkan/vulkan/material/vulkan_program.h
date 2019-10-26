
#pragma once

#include <igpu/material/program.h>
#include <framework/perf/metrics.h>
#include <framework/utility/buffer_view.h>

namespace igpu
{
	class batch;
	class batch_binding;
	class material;
	class vulkan_context;
	class vulkan_parameter;
	class vulkan_vertex_parameter;

	class vulkan_program : public program
	{
	public:

		struct config : program::config
		{
			vulkan_context* context;
		};

		const config& cfg() const override;

		~vulkan_program() override;

		size_t batch_parameter_count() const override;
		const parameter& batch_parameter(size_t) const override;

		size_t material_parameter_count() const override;
		const parameter& material_parameter(size_t) const override;

		size_t instance_parameter_count() const override;
		const parameter& instance_parameter(size_t) const override;

		size_t index_of_instance_parameter(const std::string_view&) const override;

		const primitive& default_instance_primitive(size_t instance_parameter_index) const override;

		static std::unique_ptr<vulkan_program> make(
			const config&);

	protected:

		void update(const vulkan_parameter&, const primitive&) const;

		vulkan_program(
			const config&,
			std::vector<vulkan_parameter> batch_parameters,
			std::vector<vulkan_parameter> material_parameters,
			std::vector<vulkan_parameter> instance_parameters,
			std::unordered_map<std::string_view, size_t> instance_parameter_lookup);

	private:

		const config _cfg;
		std::vector<vulkan_parameter> _batch_parameters;
		std::vector<vulkan_parameter> _material_parameters;
		std::vector<vulkan_parameter> _instance_parameters;
		std::unordered_map<std::string_view, size_t> _instance_parameter_lookup;
		perf::metric _texture_switch_metric;
		perf::metric _parameter_switch_metric;
	};
}
