
#pragma once

#include "vulkan/context/vulkan_context.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_shader.h"
#include "vulkan/shader/vulkan_vertex_parameter.h"

#include "igpu/buffer/buffer.h"
#include "igpu/shader/program_parsing.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class vulkan_shader_impl : public buffer
	{
	public:
		vulkan_shader_impl( const vulkan_shader::vulkan& );

		~vulkan_shader_impl();

		const vulkan_shader::vulkan& vk() const;

		void map( buffer_view_base* ) override;

		void unmap() override;

		VkPipelineShaderStageCreateInfo stage_info() const;

		size_t parameter_count() const;

		const parameter::config& parameter( size_t ) const;

		size_t vertex_parameter_count() const;

		const vertex_parameter::config& vertex_parameter( size_t ) const;

		VkShaderModule shader_module() const;

		void release();

	private:
		const vulkan_shader::vulkan _vk;

		VkShaderModule _shader_module = nullptr;
		std::vector< uint32_t > _memory;
		std::vector< parameter::config > _parameters;
		std::vector< vertex_parameter::config > _vertex_parameters;

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

		void map( buffer_view_base* out_buffer_view )
		{
			_shader.map( out_buffer_view );
		}

		void unmap()
		{
			_shader.unmap();
		}

		VkPipelineShaderStageCreateInfo stage_info() const override
		{
			return _shader.stage_info();
		}

		size_t parameter_count() const override
		{
			return _shader.parameter_count();
		}

		const parameter::config& parameter( size_t i ) const override
		{
			return _shader.parameter( i );
		}

		size_t vertex_parameter_count() const
		{
			return _shader.vertex_parameter_count();
		}

		const vertex_parameter::config& vertex_parameter( size_t i ) const
		{
			return _shader.vertex_parameter( i );
		}

		static std::unique_ptr< vulkan_shader_impl_t > make(
			const vulkan_shader::vulkan& vk )
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
					new vulkan_shader_impl_t( vk ) );
			}

			LOG_CRITICAL( "unhandled vulkan shader type: %d ", vk.stage_flags );

			return nullptr;
		}

	private:
		vulkan_shader_impl_t( const vulkan_shader::vulkan& vk )
			: _shader( vk )
		{}

	private:
		vulkan_shader_impl _shader;
	};
}
