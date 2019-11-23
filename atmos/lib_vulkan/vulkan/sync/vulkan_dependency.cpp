
#include "vulkan/sync/vulkan_dependency.h"

#include "vulkan/sync/vulkan_resource.h"

using namespace igpu;

vulkan_dependency::vulkan_dependency(
	vulkan_resource* resource,
	vulkan_job_dependencies* job_dependencies,
	const VkImageLayout layout,
	const vulkan_job_scope& job_scope )
	: _resource( resource )
	, _layout( layout )
	, _job_scope( job_scope )
	, _job_dependencies( job_dependencies )
	, _link( resource->add_dependency( this ) )
{
	ASSERT_CONTEXT( resource );
	ASSERT_CONTEXT( (bool)job_dependencies );
	ASSERT_CONTEXT( job_scope.validate() );
	ASSERT_CONTEXT( resource->is_valid_layout( layout ) );
}

vulkan_dependency ::~vulkan_dependency()
{
	_resource->remove_dependency( _link );
}

bool vulkan_dependency::is_staged() const
{
	return _is_staged;
}

void vulkan_dependency::is_staged( bool is_staged )
{
	_is_staged = is_staged;
}

bool vulkan_dependency::is_hazard() const
{
	return _is_hazard;
}

void vulkan_dependency::is_hazard( bool is_hazard )
{
	_is_hazard = is_hazard;
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