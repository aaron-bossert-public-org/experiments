
#pragma once


#include <vulkan/batch/vulkan_instance_batch.h>

using namespace igpu;

vulkan_instance_batch::vulkan_instance_batch(
	const config& cfg,
	const utility::sphere& visiblility_sphere)
	: _cfg(cfg)
	, _visibility_sphere(visiblility_sphere)
{
}

const vulkan_instance_batch::config& vulkan_instance_batch::cfg() const
{
	return _cfg;
}

void vulkan_instance_batch::count(size_t count)
{
	_count = count;
}

size_t vulkan_instance_batch::count() const
{
	return _count;
}

const utility::sphere& vulkan_instance_batch::visibility_sphere() const
{
	return _visibility_sphere;
}

void vulkan_instance_batch::visibility_sphere(const utility::sphere& visibility_sphere)
{
	_visibility_sphere = visibility_sphere;
}
