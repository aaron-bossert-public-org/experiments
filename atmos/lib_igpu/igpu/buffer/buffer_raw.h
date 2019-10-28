
#pragma once

#include <igpu/buffer/buffer.h>
#include <memory>
#include <framework/utility/buffer_view.h>
#include <framework/perf/metrics.h>

namespace igpu
{
	class buffer_raw : buffer
	{
	public:

		buffer_raw(
			const config&);

		const config& cfg() const override;

		void map(size_t byte_size, buffer_view_base*) override;

		void unmap() override;

		size_t byte_capacity() const override;

		const buffer_view<char>& mapped_view() const;

	protected:

		const config _cfg;
		
		buffer_view<char> _mapped_view = {};
		std::vector<char> _memory;

		perf::metric _cpu_mem_metric;
	};
}
