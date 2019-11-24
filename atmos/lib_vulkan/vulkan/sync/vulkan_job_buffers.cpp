#include "vulkan/sync/vulkan_job_buffers.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/sync/vulkan_dependency.h"
#include "vulkan/sync/vulkan_job.h"

#include "framework/logging/log.h"

using namespace igpu;

const vulkan_job_buffers::config& vulkan_job_buffers::cfg() const
{
	return _cfg;
}

vulkan_job_dependencies::state& vulkan_job_buffers::job_dependency_state()
{
	return _state;
}

const vulkan_job_dependencies::state& vulkan_job_buffers::job_dependency_state()
	const
{
	return _state;
}

vulkan_job& vulkan_job_buffers::job()
{
	return *_cfg.job;
}

const vulkan_job& vulkan_job_buffers::job() const
{
	return *_cfg.job;
}

std::unique_ptr< vulkan_job_buffers > vulkan_job_buffers::make(
	const config& cfg )
{
	if ( !cfg.job )
	{
		LOG_CRITICAL( "job is null" );
	}
	else if ( !cfg.dependencies.size() )
	{
		LOG_CRITICAL( "no dependencies provided" );
	}
	else
	{
		bool valid_dependencies = true;
		for ( size_t i = 0; i < cfg.dependencies.size(); ++i )
		{
			const auto& dependency = cfg.dependencies[i];

			if ( !dependency.buffer )
			{
				LOG_CRITICAL( "dependency buffer %d is null", (int)i );
				valid_dependencies = false;
			}

			if ( !dependency.job_scope.validate() )
			{
				LOG_CRITICAL( "job_scope %d is invalid", (int)i );
				valid_dependencies = false;
			}
		}

		if ( !valid_dependencies )
		{
			return nullptr;
		}


		auto unique = std::unique_ptr< vulkan_job_buffers >(
			new vulkan_job_buffers( cfg ) );

		auto& read_deps = unique->_state.read_deps;
		auto& write_deps = unique->_state.write_deps;

		bool success = true;

		if ( success )
		{
			size_t writable_count = 0;
			size_t readable_count = 0;
			for ( size_t i = 0; i < cfg.dependencies.size(); ++i )
			{
				const auto& dependency = cfg.dependencies[i];

				if ( 0 !=
					 ( dependency.job_scope.decorators & decorator::WRITABLE ) )
				{
					++writable_count;
				}
				else
				{
					++readable_count;
				}
			}

			write_deps.reserve( writable_count );
			read_deps.reserve( readable_count );

			for ( size_t i = 0; i < cfg.dependencies.size(); ++i )
			{
				const auto& dependency = cfg.dependencies[i];

				bool is_writable = dependency.job_scope.is_writable();

				auto& deps = is_writable ? write_deps : read_deps;

				deps.emplace_back(
					&dependency.buffer->gpu_object(),
					unique.get(),
					VK_IMAGE_LAYOUT_MAX_ENUM,
					dependency.job_scope );
			}

			ASSERT_CONTEXT(
				write_deps.size() == writable_count,
				"vector cannot be reallocated once dependencies are "
				"created" );
			ASSERT_CONTEXT(
				read_deps.size() == readable_count,
				"vector cannot be reallocated once dependencies are "
				"created" );

			return unique;
		}
	}

	return nullptr;
}

vulkan_job_buffers ::~vulkan_job_buffers()
{
	if ( is_activated() )
	{
		job().deactivate_dependencies( this );
	}
}

vulkan_job_buffers::vulkan_job_buffers( const config& cfg )
	: _cfg( cfg )
{}

void vulkan_job_buffers::record_cmds(
	const scoped_ptr< vulkan_command_buffer >& )
{}

void vulkan_job_buffers::on_resource_reinitialized( vulkan_dependency* )
{}