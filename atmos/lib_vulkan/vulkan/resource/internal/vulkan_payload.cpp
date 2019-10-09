
#include <vulkan/buffer/internal/vulkan_payload.h>

using namespace igpu;

vulkan_payload::vulkan_payload(const config& cfg)
	: _cfg(cfg)
{
}

const vulkan_payload::config& vulkan_payload::cfg() const
{
	return _cfg;
}

VkBuffer vulkan_payload::buffer() const
{
	return _buffer;
}

VmaAllocation vulkan_payload::allocation() const
{
	return _allocation;
}

VkDeviceSize vulkan_payload::size() const
{
	return _size;
}

VkAccessFlagBits vulkan_payload::access() const
{
	return _access;
}

VkShaderStageFlagBits vulkan_payload::stage() const
{
	return _stage;
}

uint32_t vulkan_payload::batch_index() const
{
	return _batch_index;
}

uint32_t vulkan_payload::queue_index() const
{
	return _queue_index;
}

void vulkan_payload::buffer(VkBuffer buffer)
{
	_buffer = buffer;
}

void vulkan_payload::size(VkDeviceSize size)
{
	_size = size;
}

void vulkan_payload::allocation(VmaAllocation allocation)
{
	_allocation = allocation;
}

void vulkan_payload::access(VkAccessFlagBits access)
{
	_access = access;
}

void vulkan_payload::stage(VkShaderStageFlagBits stage)
{
	_stage = stage;
}

void vulkan_payload::batch_index(uint32_t batch_index)
{
	_batch_index = batch_index;
}

void vulkan_payload::queue_index(uint32_t queue_index)
{
	_queue_index = queue_index;
}