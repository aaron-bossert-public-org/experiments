//
//#include "vulkan/sync/vulkan_resource.h"
//
//#include "vulkan/sync/vulkan_fence.h"
//
//#include "framework/utility/scoped_ptr.h"
//
// using namespace igpu;
//
//
// vulkan_resource::link::link(
//	const std::scoped_ptr< vulkan_resource >& resource,
//	const std::scoped_ptr< vulkan_job_dependencies >& job,
//	vulkan_access_scope )
//	: _next( resource->_read_links )
//{}
//
//~link();
//
// bool active() const;
//
// void active( bool );
//
// const vulkan_job_dependencies& job() const;
//
// vulkan_job_dependencies& job();
//
// const vulkan_resource& resource() const;
//
// vulkan_resource& resource();
//
// private:
// vulkan_resource* const _resource;
// vulkan_job_dependencies* const _job;
// const vulkan_access_scope _access_scope;
// link* _prev;
// link* _next;
//
// link( const link& ) = delete;
// link& operator=( const link& ) = delete;
//}
//;
