
#include "vulkan/sync/vulkan_dependency.h"

#include "vulkan/sync/vulkan_resource.h"

using namespace igpu;

vulkan_dependency::vulkan_dependency(
	vulkan_resource* resource,
	const VkImageLayout layout,
	const vulkan_job_scope& job_scope,
	const scoped_ptr< vulkan_job_dependencies >& job_dependencies )
	: _resource( resource )
	, _link( resource->add_dependency( this ) )
	, _layout( layout )
	, _job_scope( job_scope )
	, _job_dependencies( job_dependencies )
{}

vulkan_dependency ::~vulkan_dependency()
{
	_resource->remove_dependency( _link );
}

bool vulkan_dependency::active() const
{
	return _active;
}

void vulkan_dependency::active( bool active )
{
	_active = active;
}

VkImageLayout vulkan_dependency::layout() const
{
	return _layout;
}

const vulkan_job_dependencies& vulkan_dependency::job_dependencies() const
{
	return *_job_dependencies;
}

vulkan_job_dependencies& vulkan_dependency::job_dependencies()
{
	return *_job_dependencies;
}

const vulkan_resource& vulkan_dependency::resource() const
{
	return *_resource;
}

vulkan_resource& vulkan_dependency::resource()
{
	return *_resource;
}

const vulkan_job_scope& vulkan_dependency::job_scope() const
{
	return _job_scope;
}