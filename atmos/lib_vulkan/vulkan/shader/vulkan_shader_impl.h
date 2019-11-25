
#pragma once

#include "vulkan/context/vulkan_context.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_constant_parameters.h"
#include "vulkan/shader/vulkan_shader.h"
#include "vulkan/shader/vulkan_vertex_parameter.h"

#include "igpu/buffer/buffer.h"
#include "igpu/shader/program_parsing.h"

#include "framework/perf/metrics.h"

namespace igpu
{
	class vulkan_shader_impl
	{
	public:
		struct state
		{
			VkShaderModule shader_module = nullptr;
			std::vector< constant_parameter::config > constants;
			std::vector< parameter::config > parameters;
			std::vector< vertex_parameter::config > vertex_parameters;
		};

		vulkan_shader_impl( const vulkan_shader::vulkan&, state&& );

		~vulkan_shader_impl();

		const vulkan_shader::vulkan& vk() const;

		const state& st() const;

		VkPipelineShaderStageCreateInfo stage_info() const;

		void release();

		static bool make_state(
			const vulkan_shader::vulkan&,
			std::vector< uint32_t >&&,
			state* );

	private:
		const vulkan_shader::vulkan _vk;
		const state _st;
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

		size_t constant_count() const override
		{
			return _shader.st().constants.size();
		}

		const constant_parameter::config& constant( size_t i ) const
		{
			return _shader.st().constants[i];
		}

		size_t parameter_count() const override
		{
			return _shader.st().parameters.size();
		}

		const parameter::config& parameter( size_t i ) const override
		{
			return _shader.st().parameters[i];
		}

		size_t vertex_parameter_count() const
		{
			return _shader.st().vertex_parameters.size();
		}

		const vertex_parameter::config& vertex_parameter( size_t i ) const
		{
			return _shader.st().vertex_parameters[i];
		}

		VkShaderModule shader_module() const override
		{
			return _shader.st().shader_module;
		}

		static std::unique_ptr< vulkan_shader_impl_t > make(
			const vulkan_shader::vulkan& vk,
			std::vector< uint32_t >&& memory )
		{
			switch ( vk.stage_flags )
			{
			case VK_SHADER_STAGE_VERTEX_BIT:
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
			case VK_SHADER_STAGE_GEOMETRY_BIT:
			case VK_SHADER_STAGE_FRAGMENT_BIT:
			case VK_SHADER_STAGE_COMPUTE_BIT:
				vulkan_shader_impl::state st;
				if ( vulkan_shader_impl::make_state(
						 vk,
						 std::move( memory ),
						 &st ) )
				{
					return std::unique_ptr< vulkan_shader_impl_t >(
						new vulkan_shader_impl_t( vk, std::move( st ) ) );
				}
				else
				{
					return nullptr;
				}
			}

			LOG_CRITICAL( "unhandled vulkan shader type: %d ", vk.stage_flags );

			return nullptr;
		}

	private:
		vulkan_shader_impl_t(
			const vulkan_shader::vulkan& vk,
			vulkan_shader_impl::state&& st )
			: _shader( vk, std::move( st ) )
		{}

	private:
		vulkan_shader_impl _shader;
	};
}
