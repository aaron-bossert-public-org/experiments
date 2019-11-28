
#pragma once

#include "vulkan/shader/vulkan_constant_parameters.h"
#include "vulkan/shader/vulkan_parameters.h"

#include "igpu/shader/constants.h"

#include <array>

namespace igpu
{
	class batch;
	class raster_binding;
	class vulkan_context;
	class vulkan_poset_fence;
	class vulkan_shader;

	class vulkan_program_impl
	{
	public:
		struct config
		{
			VkDevice device;
			std::vector< std::shared_ptr< vulkan_shader > > shaders;
		};

		struct internal_config
		{
			VkPipelineLayout pipeline_layout;
			vulkan_constant_parameters::config constant_parameters;
			vulkan_parameters::config batch_parameters;
			vulkan_parameters::config material_parameters;
			vulkan_parameters::config instance_parameters;
		};

		const config& cfg() const;

		const vulkan_constant_parameters& constant_parameters() const;

		const vulkan_parameters& batch_parameters() const;

		const vulkan_parameters& material_parameters() const;

		const vulkan_parameters& instance_parameters() const;

		VkPipelineLayout pipeline_layout() const;

		static bool make_internal_cfg( const config&, internal_config* );

		vulkan_program_impl( const config&, internal_config&& );

		~vulkan_program_impl();

	private:
		const config _cfg;
		vulkan_constant_parameters _constant_parameters;
		vulkan_parameters _batch_parameters;
		vulkan_parameters _material_parameters;
		vulkan_parameters _instance_parameters;
		VkPipelineLayout _pipeline_layout = nullptr;
	};

	template < typename T >
	class vulkan_program_impl_t : public T
	{
	public:
		const vulkan_constant_parameters& constant_parameters() const override
		{
			return _program_impl.constant_parameters();
		}

		const vulkan_parameters& batch_parameters() const override
		{
			return _program_impl.batch_parameters();
		}

		const vulkan_parameters& material_parameters() const override
		{
			return _program_impl.material_parameters();
		}

		const vulkan_parameters& instance_parameters() const override
		{
			return _program_impl.instance_parameters();
		}

		VkPipelineLayout pipeline_layout() const
		{
			return _program_impl.pipeline_layout();
		}

		template < typename... ARGS >
		vulkan_program_impl_t(
			const vulkan_program_impl::config& cfg,
			vulkan_program_impl::internal_config&& internal,
			ARGS&&... args )
			: T( std::forward< ARGS >( args )... )
			, _program_impl( cfg, std::move( internal ) )
		{}

	private:
		vulkan_program_impl _program_impl;
	};
}
