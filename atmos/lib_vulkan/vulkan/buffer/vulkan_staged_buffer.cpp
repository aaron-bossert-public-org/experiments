
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

void vulkan_staged_buffer::map(buffer_view_base& buffer_view, size_t byte_size)
{
	if (_staging_buffer)
	{
		LOG_WARNING("map/unmap mismatch");
		unmap();
	}

	_staging_buffer = vulkan_buffer::make({
			_cfg.buffer_mediator.lock()->vma(),
			VMA_MEMORY_USAGE_CPU_ONLY,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			byte_size,
		});

	if (_staging_buffer)
	{
		buffer_view = buffer_view_base(
			byte_size / buffer_view.stride(),
			_staging_buffer->map(),
			buffer_view.stride());
	}
	else
	{
		LOG_CRITICAL("failed to create staging buffer");
		
		buffer_view = buffer_view_base(
			0,
			nullptr,
			buffer_view.stride());
	}
}

void vulkan_staged_buffer::unmap()
{
	auto buffer_mediator = _cfg.buffer_mediator.lock();
	if (!buffer_mediator)
	{
		LOG_CRITICAL("buffer mediator has expired");
	}
	else if (!_staging_buffer)
	{
		LOG_CRITICAL("staging buffer is null");
	}
	else 
	{
		_staging_buffer->unmap();

		if (!_gpu_buffer || _gpu_buffer->cfg().size < _staging_buffer->cfg().size)
		{
			_gpu_buffer = vulkan_buffer::make({
				buffer_mediator->vma(),
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
			buffer_mediator->copy(_staging_buffer, _gpu_buffer);
			
			_staging_buffer = nullptr;
		}
	} 
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

size_t vulkan_staged_buffer::byte_size() const
{
	return _gpu_buffer->cfg().size;
}

bool vulkan_staged_buffer::validate(const config& cfg)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CRITICAL("unhandled usage:%s", to_string(cfg.usage).data());
	}
	else if (0 == cfg.vk_usage_flags)
	{
		LOG_CRITICAL("vk_usage_flags is 0");
	}
	else if (nullptr == cfg.buffer_mediator.lock())
	{
		LOG_CRITICAL("buffer mediator has expired");
	}
	else
	{
		return true;
	}

	return false;
}

