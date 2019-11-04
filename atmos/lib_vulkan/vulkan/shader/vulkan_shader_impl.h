
#pragma once

#include "vulkan/buffer/vulkan_vertex_parameter.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_shader.h"

#include "igpu/buffer/vector_buffer.h"
#include "igpu/shader/program_parsing.h"

#include "framework/perf/metrics.h"
namespace igpu
{
	class vulkan_shader_impl
	{
	public:
		vulkan_shader_impl(
			const vulkan_shader::vulkan&,
			vector_buffer< uint32_t >&& );

		~vulkan_shader_impl();

		const vulkan_shader::vulkan& vk() const;

		VkPipelineShaderStageCreateInfo stage_info() const;

		size_t parameter_count() const;

		const spirv::parameter& parameter( size_t ) const;

		size_t vertex_parameter_count() const;

		const spirv::vertex_parameter& vertex_parameter( size_t ) const;

		VkShaderModule shader_module() const;

	private:
		const vulkan_shader::vulkan _vk;
		VkShaderModule _shader_module = nullptr;
		std::vector< spirv::parameter > _parameters;
		std::vector< spirv::vertex_parameter > _vertex_parameters;

		perf::metric _cpu_mem_metric;
	};

	template < typename T >
	class vulkan_shader_impl_t : public T
	{
	public:
		const vulkan_shader::vulkan& vk() const override
		{
			return _shader.vk();
		}

		VkPipelineShaderStageCreateInfo stage_info() const override
		{
			return _shader.stage_info();
		}

		size_t parameter_count() const override
		{
			return _shader.parameter_count();
		}

		const spirv::parameter& parameter( size_t i ) const override
		{
			return _shader.parameter( i );
		}

		size_t vertex_parameter_count() const
		{
			return _shader.vertex_parameter_count();
		}

		const spirv::vertex_parameter& vertex_parameter( size_t i ) const
		{
			return _shader.vertex_parameter( i );
		}

		static std::unique_ptr< vulkan_shader_impl_t > make(
			const vulkan_shader::vulkan& vk,
			vector_buffer< uint32_t >&& buffer )
		{
			if ( buffer.view_unmapped().size() == 0 )
			{
				LOG_CRITICAL( "raw buffer is empty or still mapped" );
			}
			else
			{
				switch ( vk.stage_flags )
				{
				case VK_SHADER_STAGE_VERTEX_BIT:
				case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
				case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
				case VK_SHADER_STAGE_GEOMETRY_BIT:
				case VK_SHADER_STAGE_FRAGMENT_BIT:
				case VK_SHADER_STAGE_COMPUTE_BIT:
					return std::unique_ptr< vulkan_shader_impl_t >(
						new vulkan_shader_impl_t( vk, std::move( buffer ) ) );
				}

				LOG_CRITICAL(
					"unhandled vulkan shader type: %d ",
					vk.stage_flags );
			}

			return nullptr;
		}

	private:
		vulkan_shader_impl_t(
			const vulkan_shader::vulkan& vk,
			vector_buffer< uint32_t >&& buffer )
			: _shader( vk, std::move( buffer ) )
		{}

	private:
		vulkan_shader_impl _shader;
	};
}
