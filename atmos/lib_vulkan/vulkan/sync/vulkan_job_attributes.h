
#pragma once

#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_job_dependencies.h"

namespace igpu
{
	class vulkan_geometry;
	class vulkan_graphics_pipeline;

	class vulkan_job_attributes : public vulkan_job_dependencies
	{
	public:
		struct private_ctor;

		struct config
		{
			VkDevice device = nullptr;
			vulkan_job* job = nullptr;
			size_t swap_count = 0;
			std::vector< size_t > active_vertex_buffers;
			vulkan_geometry* geometry = nullptr;
		};

		const config& cfg() const;

		static std::shared_ptr< vulkan_job_attributes > make( const config& );

		vulkan_job_attributes( const private_ctor& );

		~vulkan_job_attributes();

	protected:
		vulkan_job& job() override;

		const vulkan_job& job() const override;

		state& job_dependency_state() override;

		const state& job_dependency_state() const override;

		void on_record_cmds(
			const scoped_ptr< vulkan_command_buffer >& ) override;

		void on_resource_reinitialized( vulkan_dependency* ) override;

	private:
		const config _cfg;
		state _state;

		VkBuffer _index_buffer = nullptr;
		VkDeviceSize _index_buffer_offset = (VkDeviceSize)-1;
		VkIndexType _index_type = VK_INDEX_TYPE_MAX_ENUM;

		std::vector< VkBuffer > _vertex_buffers;
		std::vector< VkDeviceSize > _vertex_buffer_offsets;
	};
}
