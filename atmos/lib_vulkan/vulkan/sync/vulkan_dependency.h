
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
			vulkan_job_dependencies*,
			const VkImageLayout,
			const vulkan_job_scope& );

		~vulkan_dependency();

		bool is_staged() const;

		void is_staged( bool );

		bool is_hazard() const;

		void is_hazard( bool );

		VkImageLayout layout() const;

		const vulkan_job_dependencies& job_dependencies() const;

		vulkan_job_dependencies& job_dependencies();

		const vulkan_resource& resource() const;

		vulkan_resource& resource();

		const vulkan_job_scope& job_scope() const;

	private:
		bool _is_staged = false;
		bool _is_hazard = false;
		vulkan_resource* const _resource;
		vulkan_job_dependencies* const _job_dependencies;
		const VkImageLayout _layout;
		const vulkan_job_scope _job_scope;
		const vulkan_resource::link _link;
	};
}