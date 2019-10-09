
#include <vulkan/resource/vulkan_compute_resource.h>

// vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// vulkan implementation includes - end

using namespace igpu;


const compute_resource::config& vulkan_compute_resource::cfg() const
{
	return _cfg;
}

void vulkan_compute_resource::map(buffer_view_base& resource_view, size_t count)
{
	_vulkan_resource.map(resource_view, count);
}

void vulkan_compute_resource::map_subrange(buffer_view_base& mapped, size_t offset, size_t count)
{
	_vulkan_resource.map_subrange(mapped, offset, count);
}

void vulkan_compute_resource::unmap(buffer_view_base& resource_view)
{
	_vulkan_resource.unmap(resource_view);
}

size_t vulkan_compute_resource::capacity() const
{
	return _vulkan_resource.capacity();
}

std::unique_ptr<vulkan_compute_resource> vulkan_compute_resource::make(const config& cfg)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "unhandled usage:%s", to_string(cfg.usage).data());
	}
	else
	{
		return std::unique_ptr<vulkan_compute_resource>(new vulkan_compute_resource(cfg));
	}

	return nullptr;
}

vulkan_compute_resource::vulkan_compute_resource(const config& cfg)
	: _cfg(cfg)
	, _vulkan_resource(
		"gpu compute mem",
		{ cfg.usage, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT })
{
}
