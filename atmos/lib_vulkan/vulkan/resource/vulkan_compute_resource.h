
#pragma once

#include <framework/perf/metrics.h>

#include <igpu/resource/compute_resource.h>

#include <vulkan/resource/internal/vulkan_resource.h>

namespace igpu
{
	class vulkan_compute_resource : public compute_resource
	{
	public:

		~vulkan_compute_resource() override {};

		const config& cfg() const override;

		void map(buffer_view_base&, size_t count) override;

		void map_subrange(buffer_view_base&, size_t offset, size_t count) override;

		void unmap(buffer_view_base&) override;

		size_t capacity() const override;

		static std::unique_ptr<vulkan_compute_resource> make(const config&);
	};
}
