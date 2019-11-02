
#pragma once


#include <vulkan/batch/vulkan_instance_batch.h>

using namespace igpu;

vulkan_instance_batch::vulkan_instance_batch(
	const config&, // cfg,
	const utility::sphere& visiblility_sphere)
	: _visibility_sphere(visiblility_sphere)
{
}

void vulkan_instance_batch::element_start(const std::optional < size_t >& element_start)
{
	_element_start = element_start;
}

const std::optional < size_t >& vulkan_instance_batch::element_start() const
{
	return _element_start;
}

void vulkan_instance_batch::element_count(const std::optional < size_t >& element_count)
{
	_element_count = element_count;
}

const std::optional < size_t >& vulkan_instance_batch::element_count() const
{
	return _element_count;
}

void vulkan_instance_batch::instance_count(const std::optional < size_t >& instance_count)
{
	_instance_count = instance_count;
}

const std::optional < size_t >& vulkan_instance_batch::instance_count() const
{
	return _instance_count;
}

const utility::sphere& vulkan_instance_batch::visibility_sphere() const
{
	return _visibility_sphere;
}

void vulkan_instance_batch::visibility_sphere(const utility::sphere& visibility_sphere)
{
	_visibility_sphere = visibility_sphere;
}
