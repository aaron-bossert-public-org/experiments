
#include <vulkan/resource/vulkan_vertex_resource.h>

// vulkan implementation includes - begin
#include <vulkan/defines/vulkan_includes.h>
// vulkan implementation includes - end

using namespace igpu;

const vertex_resource::config& vulkan_vertex_resource::cfg() const
{
	return _cfg;
}

void vulkan_vertex_resource::map(buffer_view_base& resource_view, size_t count)
{
	_vulkan_resource.map(resource_view, count);
}

void vulkan_vertex_resource::map_subrange(buffer_view_base& resource_view, size_t offset, size_t count)
{
	_vulkan_resource.map_subrange(resource_view, offset, count);
}

void vulkan_vertex_resource::unmap(buffer_view_base& resource_view)
{
	_vulkan_resource.unmap(resource_view);
}

size_t vulkan_vertex_resource::capacity() const
{
	return _vulkan_resource.capacity();
}

std::unique_ptr<vulkan_vertex_resource> vulkan_vertex_resource::make(const config& cfg)
{
	if (!is_valid(cfg.usage))
	{
		LOG_CONTEXT(CRITICAL, "unhandled usage:%s", to_string(cfg.usage).data());
	}
	else
	{
		return std::unique_ptr<vulkan_vertex_resource>(new vulkan_vertex_resource(cfg));
	}

	return nullptr;
}

vulkan_vertex_resource::vulkan_vertex_resource(const config& cfg)
	: _cfg(cfg)
	, _vulkan_resource(
		"gpu vertex mem",
		{ cfg.usage, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT })
{
}
