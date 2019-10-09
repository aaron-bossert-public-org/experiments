
#pragma once

#include <framework/perf/metrics.h>

#include <igpu/resource/index_resource.h>
#include <igpu/resource/index_format.h>

#include <vulkan/resource/internal/vulkan_resource.h>

namespace igpu
{
	class vulkan_index_resource : public index_resource
	{
	public:

		~vulkan_index_resource() override {};

		const config& cfg() const override;

		void map(buffer_view_base&, size_t count) override;

		void map_subrange(buffer_view_base&, size_t offset, size_t count) override;

		void unmap(buffer_view_base&) override;

		size_t capacity() const override;

		static std::unique_ptr<vulkan_index_resource> make(const config&);

	protected:

		vulkan_index_resource(const config&);

	private:

		const config _cfg;
		vulkan_resource _vulkan_resource;
	};
}
