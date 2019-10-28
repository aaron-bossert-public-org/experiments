

#include <igpu/buffer/buffer_raw.h>

#include <framework/logging/log.h>

using namespace igpu;

buffer_raw::buffer_raw(
	const config& cfg)
	: _cfg(cfg)
	, _cpu_mem_metric(perf::category::CPU_MEM_USAGE, "raw buffer")
{
}

const buffer_raw::config& buffer_raw::cfg() const
{
	return _cfg;
}

void buffer_raw::map(size_t byte_size, buffer_view_base* out_mapped_view)
{
	if (_mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		_memory.reserve(byte_size);
		_mapped_view = buffer_view<char>(
			_memory.capacity(), 
			_memory.data());

		_cpu_mem_metric.reset();
		_cpu_mem_metric.add(_mapped_view.byte_size());

		size_t stride = out_mapped_view->stride();
		*out_mapped_view = buffer_view_base(
			byte_size / stride,
			_memory.data(),
			stride);
	}
}

void buffer_raw::unmap()
{
	if (!_mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		if (_cfg.usage == buffer_usage::STATIC)
		{
			_cpu_mem_metric.reset();
			_memory.shrink_to_fit();
		}

		_mapped_view = buffer_view<char>(
			_memory.capacity(),
			nullptr);
	}
}

size_t buffer_raw::byte_capacity() const
{
	return _memory.capacity();
}

const buffer_view<char>& buffer_raw::mapped_view() const
{
	return _mapped_view;
}
