
#include "vulkan/sync/vulkan_job_scope.h"

using namespace igpu;

bool vulkan_job_scope::is_readable() const
{
	return 0 != ( decorators & decorator::READABLE );
}

bool vulkan_job_scope::is_writable() const
{
	return 0 != ( decorators & decorator::WRITABLE );
}

bool vulkan_job_scope::contains( const vulkan_job_scope& other ) const
{
	return decorators == ( decorators | other.decorators ) &&
		stages == ( stages | other.stages ) &&
		access == ( access | other.access );
}

vulkan_job_scope vulkan_job_scope::combined(
	const vulkan_job_scope& other ) const
{
	return {
		decorators | other.decorators,
		stages | other.stages,
		access | other.access,
	};
}