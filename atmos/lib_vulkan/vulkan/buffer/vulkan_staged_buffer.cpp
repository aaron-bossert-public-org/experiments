
#include <vulkan/buffer/vulkan_staged_buffer.h>

// Vulkan implementation includes - begin
#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/buffer/vulkan_buffer_mediator.h>
#include <vulkan/defines/vulkan_includes.h>
// Vulkan implementation includes - end

#include <framework/utility/buffer_view.h>
using namespace igpu;

vulkan_staged_buffer::vulkan_staged_buffer(
	const config& cfg)
	: _cfg(cfg)
{
}

vulkan_staged_buffer::~vulkan_staged_buffer()
{
}

const vulkan_staged_buffer::config& vulkan_staged_buffer::cfg() const
{
	return _cfg;
}

void vulkan_staged_buffer::map(size_t byte_size, buffer_view_base* out_buffer_view)
{
	_byte_size = (uint32_t)byte_size;
	if (!_staging_buffer || _staging_buffer->cfg().size < byte_size)
	{
		_staging_buffer = vulkan_buffer::make({
				_cfg.buffer_mediator->vma(),
				VMA_MEMORY_USAGE_CPU_ONLY,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				byte_size,
			});
	}

	if (_staging_buffer)
	{
		*out_buffer_view = buffer_view_base(
			byte_size / out_buffer_view->stride(),
			_staging_buffer->map(),
			out_buffer_view->stride());
	}
	else
	{
		LOG_CRITICAL("failed to create staging buffer");
		
		*out_buffer_view = buffer_view_base(
			0,
			nullptr,
			out_buffer_view->stride());
	}
}

void vulkan_staged_buffer::unmap()
{
	if (!_staging_buffer)
	{
		LOG_CRITICAL("staging buffer is null");
	}
	else 
	{
		auto& buffer_mediator = *_cfg.buffer_mediator;

		_staging_buffer->unmap();
		 
		if (!_gpu_buffer || _gpu_buffer->cfg().size < _staging_buffer->cfg().size)
		{
			_gpu_buffer = vulkan_buffer::make({
				buffer_mediator.vma(),
				VMA_MEMORY_USAGE_GPU_ONLY,
				VkBufferUsageFlagBits(VK_BUFFER_USAGE_TRANSFER_DST_BIT | _cfg.vk_usage_flags),
				_staging_buffer->cfg().size,
				});
		}
		
		if (!_gpu_buffer)
		{
			LOG_CRITICAL("failed to create gpu buffer");
		}
		else
		{
			buffer_mediator.copy(*_staging_buffer, *_gpu_buffer, _byte_size);
		}

		if (_cfg.usage == buffer_usage::STATIC)
		{
			_staging_buffer = nullptr;
		}
	} 
}

size_t vulkan_staged_buffer::byte_size() const
{
	return _byte_size;
}

void vulkan_staged_buffer::release()
{
	if (_staging_buffer)
	{
		LOG_WARNING("releasing buffer while staging memory is mapped, unmapping now to avoid internal inconsistancies");
		_staging_buffer->unmap();
		_staging_buffer = nullptr;
	}

	_gpu_buffer = nullptr;
}

VkBuffer vulkan_staged_buffer::get()
{
	return _gpu_buffer->get();
}