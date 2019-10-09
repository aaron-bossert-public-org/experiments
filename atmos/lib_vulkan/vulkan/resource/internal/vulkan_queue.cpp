
#include <vulkan/buffer/internal/vulkan_queue.h>

using namespace igpu;

vulkan_queue::vulkan_queue(const config& cfg)
	: _cfg(cfg)
{
}

const vulkan_queue::config& vulkan_queue::cfg() const
{
	return _cfg;
}

uint32_t vulkan_queue::batch_index() const
{
	return _batch_index;
}

void vulkan_queue::batch_index(uint32_t batch_index)
{
	_batch_index = batch_index;
}