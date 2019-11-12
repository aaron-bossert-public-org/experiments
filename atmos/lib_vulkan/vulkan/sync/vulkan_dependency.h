
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
			const VkImageLayout,
			const vulkan_job_scope&,
			const scoped_ptr< vulkan_job_dependencies >&,
			const vulkan_parameter::config& );

		vulkan_dependency();

		bool active() const;

		void active( bool );

		VkImageLayout layout() const;

		const vulkan_job_dependencies& job() const;

		vulkan_job_dependencies& job();

		const vulkan_resource& resource() const;

		vulkan_resource& resource();

		const vulkan_job_scope& job_scope() const;

		const vulkan_parameter::config& parameter_config() const;

	private:
		const vulkan_resource::link _link;
		const VkImageLayout _layout;
		const vulkan_job_scope _job_scope;
		scoped_ptr< vulkan_job_dependencies > _job;
		scoped_ptr< vulkan_resource > _resource;
		vulkan_parameter::config _parameter_config;
	};
}