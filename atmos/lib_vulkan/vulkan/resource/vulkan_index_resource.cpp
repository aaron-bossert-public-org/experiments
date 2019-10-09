
#include <vulkan/buffer/vulkan_index_resource.h>

// vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// vulkan implementation includes - end

using namespace igpu;


const index_resource::config& vulkan_index_resource::cfg() const
{
	return _cfg;
}

void vulkan_index_resource::map(buffer_view_base& mapped, size_t count)
{
	_vulkan_resource.map(mapped, count);
}

void vulkan_index_resource::map_subrange(buffer_view_base& mapped, size_t offset, size_t count)
{
	_vulkan_resource.map_subrange(mapped, offset, count);
}

void vulkan_index_resource::unmap(buffer_view_base& mapped)
{
	_vulkan_resource.unmap(mapped);
}

size_t vulkan_index_resource::capacity() const
{
	return _vulkan_resource.capacity();
}

std::unique_ptr<vulkan_index_resource> vulkan_index_resource::make(const config& cfg)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "unhandled usage:%s", to_string(cfg.usage).data());
	}
	else
	{
		return std::unique_ptr<vulkan_index_resource>(new vulkan_index_resource(cfg));
	}

	return nullptr;
}

vulkan_index_resource::vulkan_index_resource(const config& cfg)
	: _cfg(cfg)
	, _vulkan_resource(
		"gpu index mem",
		{ cfg.usage, VK_BUFFER_USAGE_INDEX_BUFFER_BIT })
{
}
