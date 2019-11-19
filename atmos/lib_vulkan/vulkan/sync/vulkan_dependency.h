
#pragma once

#include "vulkan/sync/vulkan_job_scope.h"
#include "vulkan/sync/vulkan_resource.h"

#include "framework/utility/scoped_ptr.h"

namespace igpu
{
	class vulkan_job_dependencies;
	class vulkan_resource;

	class vulkan_dependency
	{
	public:
		vulkan_dependency(
			vulkan_resource*,
			const VkImageLayout,
			const vulkan_job_scope&,
			const scoped_ptr< vulkan_job_dependencies >& );

		~vulkan_dependency();

		bool active() const;

		void active( bool );

		VkImageLayout layout() const;

		const vulkan_job_dependencies& job_dependencies() const;

		vulkan_job_dependencies& job_dependencies();

		const vulkan_resource& resource() const;

		vulkan_resource& resource();

		const vulkan_job_scope& job_scope() const;

	private:
		bool _active = false;
		vulkan_resource* const _resource;
		const VkImageLayout _layout;
		const vulkan_job_scope _job_scope;
		const scoped_ptr< vulkan_job_dependencies > _job_dependencies;
		const vulkan_resource::link _link;
	};
}