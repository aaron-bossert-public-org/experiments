
#pragma once

#include "igpu/batch/batch_binding.h"
#include "igpu/batch/instance_batch.h"

namespace igpu
{
	struct vulkan_batch_raster_state;
	class vulkan_job_primitives;

	class vulkan_root_batch;
	class vulkan_program;
	class vulkan_primitives;

	class vulkan_instance_batch : public instance_batch
	{
	public:
		struct config : instance_batch::config
		{
			struct vulkan
			{
				vulkan_root_batch* root_batch = nullptr;
				std::shared_ptr< vulkan_program > program;
				std::shared_ptr< vulkan_primitives > instance;
			};

			vulkan vk;
		};

		vulkan_instance_batch( const config& cfg );

		void enabled( bool ) override;

		void base_vertex( const std::optional< ptrdiff_t >& ) override;

		void instance_start( const std::optional< size_t >& ) override;

		void instance_count( const std::optional< size_t >& ) override;

		void element_start( const std::optional< size_t >& ) override;

		void element_count( const std::optional< size_t >& ) override;

		void visibility_sphere(
			const std::optional< utility::sphere >& ) override;

		bool enabled() const override;

		const std::optional< ptrdiff_t >& base_vertex() const override;

		const std::optional< size_t >& instance_start() const override;

		const std::optional< size_t >& instance_count() const override;

		const std::optional< size_t >& element_start() const override;

		const std::optional< size_t >& element_count() const override;

		const std::optional< utility::sphere >& visibility_sphere()
			const override;

		bool can_raster( vulkan_batch_raster_state* );

		void rasterize( const vulkan_batch_raster_state& );

		vulkan_instance_batch() = default;
		vulkan_instance_batch( vulkan_instance_batch&& ) = default;
		vulkan_instance_batch& operator=( vulkan_instance_batch&& ) = default;

	private:
		bool _enabled = true;
		std::optional< ptrdiff_t > _base_vertex;
		std::optional< size_t > _instance_start;
		std::optional< size_t > _instance_count;
		std::optional< size_t > _element_start;
		std::optional< size_t > _element_count;

#pragma warning( push )
#pragma warning( disable : 4324 )
		//  warning C4324: 'std::_Optional_destruct_base<_Ty,true>': structure
		//  was padded due to alignment specifier
		std::optional< utility::sphere > _visibility_sphere;
#pragma warning( pop )

		std::shared_ptr< vulkan_job_primitives > _job_primitives;
	};
}
