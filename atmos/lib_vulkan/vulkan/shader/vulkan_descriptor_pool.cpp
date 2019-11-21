
#include "vulkan/shader/vulkan_descriptor_pool.h"

#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/shader/vulkan_parameters.h"
#include "vulkan/sync/vulkan_queue.h"

#include <array>

using namespace igpu;

namespace
{
	static vulkan_descriptor_pool::config make_config(
		const scoped_ptr< vulkan_queue >& queue,
		VkDescriptorPoolCreateFlags flags,
		const vulkan_parameters& parameters,
		uint32_t max_sets )
	{
		uint32_t uniform_buffers = 0;
		uint32_t storage_buffers = 0;
		uint32_t samplers = 0;

		for ( int i = 0; i < parameters.count(); ++i )
		{
			const auto& cfg = parameters.parameter( i ).cfg();
			switch ( cfg.type )
			{
			case parameter::type::UNIFORM_BUFFER:
				uniform_buffers += (uint32_t)cfg.array_size;
				break;

			case parameter::type::STORAGE_BUFFER:
				storage_buffers += (uint32_t)cfg.array_size;
				break;

			case parameter::type::SAMPLER2D:
				samplers += (uint32_t)cfg.array_size;
				break;
			}
		}

		vulkan_descriptor_pool::config cfg = {
			queue,
			flags,
			parameters.cfg().vk.layout,
			max_sets,
		};

		if ( uniform_buffers )
		{
			cfg.sizes[cfg.size_count++] = {
				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				uniform_buffers * max_sets,
			};
		}

		if ( storage_buffers )
		{
			cfg.sizes[cfg.size_count++] = {
				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
				storage_buffers * max_sets,
			};
		}

		if ( samplers )
		{
			cfg.sizes[cfg.size_count++] = {
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				samplers * max_sets,
			};
		}

		return cfg;
	}
}

const vulkan_descriptor_pool::config& vulkan_descriptor_pool::cfg() const
{
	return _cfg;
}

VkDescriptorSet vulkan_descriptor_pool::allocate_descriptor_set()
{
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool = _vk_pool;
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &_cfg.layout;

	VkDescriptorSet set = nullptr;
	vkAllocateDescriptorSets( _device, &alloc_info, &set );
	return set;
}

std::unique_ptr< vulkan_descriptor_pool > vulkan_descriptor_pool::make(
	const scoped_ptr< vulkan_queue >& queue,
	VkDescriptorPoolCreateFlags flags,
	const vulkan_parameters& parameters,
	uint32_t max_sets )
{
	config cfg = make_config( queue, flags, parameters, max_sets );

	return make( cfg );
}

std::unique_ptr< vulkan_descriptor_pool > vulkan_descriptor_pool::make(
	const config& cfg )
{
	auto queue = cfg.queue;

	if ( !queue )
	{
		LOG_CRITICAL( "queue is null" );
	}
	else
	{
		VkDescriptorPoolCreateInfo pool_info = {
			VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			nullptr,
			cfg.flags,
			cfg.max_sets,
			cfg.size_count,
			cfg.sizes.data(),
		};

		VkDevice device = cfg.queue->cfg().device;
		VkDescriptorPool vk_pool = nullptr;
		vkCreateDescriptorPool( device, &pool_info, nullptr, &vk_pool );

		if ( !vk_pool )
		{
			LOG_CRITICAL( "descriptor pool is null" );
		}
		else
		{
			return std::unique_ptr< vulkan_descriptor_pool >(
				new vulkan_descriptor_pool( cfg, device, vk_pool ) );
		}
	}

	return nullptr;
}

vulkan_descriptor_pool::vulkan_descriptor_pool(
	const config& cfg,
	VkDevice device,
	VkDescriptorPool vk_pool )
	: _cfg( cfg )
	, _device( device )
	, _vk_pool( vk_pool )
{}

vulkan_descriptor_pool::~vulkan_descriptor_pool()
{
	if ( _vk_pool )
	{
		abandon( _cfg.queue, _device, _vk_pool );
	}
}
