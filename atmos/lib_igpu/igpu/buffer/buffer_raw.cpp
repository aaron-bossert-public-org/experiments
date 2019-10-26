

#include <igpu/buffer/buffer_raw.h>

#include <framework/logging/log.h>

using namespace igpu;

buffer_raw::buffer_raw(
	const config& cfg)
	: _cfg(cfg)
	, _cpu_mem_metric(perf::category::CPU_MEM_USAGE, "raw buffer")
{
}

void buffer_raw::map(size_t byte_size, buffer_view_base* out_mapped_view)
{
	if (_mapped_view.data())
	{
		LOG_CRITICAL("map/unmap mismatch");
	}
	else
	{
		if (!_memory || _mapped_view.size() < byte_size)
		{
			_cpu_mem_metric.reset();
			_cpu_mem_metric.add(byte_size);
			_memory.reset(new char[byte_size]);
			_mapped_view = buffer_view<char>(
				byte_size, 
				_memory.get());
		}
		else
		{
			_mapped_view = buffer_view<char>(
				_mapped_view.byte_size(),
				_memory.get());
		}

		size_t stride = out_mapped_view->stride();
		*out_texture_data = buffer_view_base(
			byte_size / stride,
			_memory.get(),
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
		_mapped_view = buffer_view<char>(
			_mapped_view.byte_size(),
			nullptr);

		if (_cfg.usage == buffer_usage::STATIC)
		{
			_cpu_mem_metric.reset();
			_memory.reset();
		}
	}
}

size_t buffer_raw::byte_size() const
{
	return _mapped_view.byte_size();
}


const buffer_view<char>& buffer_raw::mapped_view() const
{
	return _mapped_view;
}
