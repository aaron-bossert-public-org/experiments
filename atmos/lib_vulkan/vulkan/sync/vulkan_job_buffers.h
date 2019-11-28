
#pragma once

#include "vulkan/compute/vulkan_compute_buffer.h"
#include "vulkan/sync/vulkan_job_dependencies.h"
#include "vulkan/sync/vulkan_job_scope.h"

namespace igpu
{
	class vulkan_compute_buffer;

	class vulkan_job_buffers : public vulkan_job_dependencies
	{
	public:
		struct buffer_dependency
		{
			vulkan_job_scope job_scope;
			scoped_ptr< vulkan_compute_buffer > buffer;
		};

		struct config
		{
			vulkan_job* job = nullptr;
			std::vector< buffer_dependency > dependencies;
		};

		~vulkan_job_buffers();

		static std::unique_ptr< vulkan_job_buffers > make( const config& );

		const config& cfg() const;

		void record_cmds( const scoped_ptr< vulkan_command_buffer >& ) override;

	protected:
		vulkan_job_buffers( const config& );

		vulkan_job& job() override;

		const vulkan_job& job() const override;

		state& job_dependency_state() override;

		const state& job_dependency_state() const override;

		void on_resource_reinitialized( vulkan_dependency* ) override;

	private:
		const config _cfg;
		state _state;
	};
}
