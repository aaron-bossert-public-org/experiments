
#include <vulkan/buffer/vulkan_meta_queue.h>

using namespace igpu;

vulkan_meta_queue::vulkan_meta_queue(const config& cfg)
	: _cfg(cfg)
{
}

const vulkan_meta_queue::config& vulkan_meta_queue::cfg() const
{
	return _cfg;
}

uint32_t vulkan_meta_queue::batch_index() const
{
	return _batch_index;
}

void vulkan_meta_queue::batch_index(uint32_t batch_index)
{
	_batch_index = batch_index;
}