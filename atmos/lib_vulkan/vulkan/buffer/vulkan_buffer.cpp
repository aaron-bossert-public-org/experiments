
#include "vulkan/buffer/vulkan_buffer.h"

#include "vulkan/shader/vulkan_parameter.h"
#include "vulkan/sync/vulkan_queue.h"

using namespace igpu;

namespace
{
	std::string to_string( VkBufferUsageFlagBits bits )
	{
		std::string str;

		if ( bits & VK_BUFFER_USAGE_TRANSFER_SRC_BIT )
			str += "transfer src";
		if ( bits & VK_BUFFER_USAGE_TRANSFER_DST_BIT )
			str += "transfer dst";
		if ( bits & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT )
			str += "uniform texel";
		if ( bits & VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT )
			str += "storage texel";
		if ( bits & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT )
			str += "uniform buffer";
		if ( bits & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT )
			str += "storage buffer";
		if ( bits & VK_BUFFER_USAGE_INDEX_BUFFER_BIT )
			str += "index buffer";
		if ( bits & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT )
			str += "vertex buffer";
		if ( bits & VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT )
			str += "indirect buffer";
		if ( bits & VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_BUFFER_BIT_EXT )
			str += "transform feedback";
		if ( bits & VK_BUFFER_USAGE_TRANSFORM_FEEDBACK_COUNTER_BUFFER_BIT_EXT )
			str += "transform feedback";
		if ( bits & VK_BUFFER_USAGE_CONDITIONAL_RENDERING_BIT_EXT )
			str += "conditional rendering";
		if ( bits & VK_BUFFER_USAGE_RAY_TRACING_BIT_NV )
			str += "ray tracing";
		if ( bits & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT_EXT )
			str += "shader device";

		return str;
	}

	perf::category to_category( VmaMemoryUsage usage )
	{
		switch ( usage )
		{
		case VMA_MEMORY_USAGE_GPU_ONLY:
			return perf::category::GPU_MEM_USAGE;
		case VMA_MEMORY_USAGE_CPU_ONLY:
			return perf::category::CPU_MEM_USAGE;
		case VMA_MEMORY_USAGE_CPU_TO_GPU:
			return perf::category::CPU_GPU_SHARED_MEM_USAGE;
		case VMA_MEMORY_USAGE_GPU_TO_CPU:
			return perf::category::CPU_GPU_SHARED_MEM_USAGE;
		};

		return perf::category::UNDEFINED;
	}
}

vulkan_buffer::vulkan_buffer( const config& cfg )
	: _cfg( cfg )
	, _mem_metric(
		  to_category( cfg.vk.vma_usage ),
		  ::to_string( cfg.vk.usage ) )
{}

vulkan_buffer::~vulkan_buffer()
{
	vulkan_resource::wait_pending_jobs();

	release();
}

const vulkan_buffer::config& vulkan_buffer::cfg() const
{
	return _cfg;
}


VkBuffer vulkan_buffer::vk_buffer() const
{
	return _buffer;
}

void vulkan_buffer::map( buffer_view_base* out_buffer_view )
{
	if ( _mapped_view.data() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		reserve( out_buffer_view->byte_size() );

		if ( !_buffer )
		{
			LOG_CRITICAL( "failed to create staging buffer" );

			_mapped_view = *out_buffer_view =
				buffer_view_base( 0, nullptr, out_buffer_view->stride() );
		}
		else
		{
			void* mapped;
			vmaMapMemory( _cfg.vk.vma, _vma_allocation, &mapped );

			_mapped_view =
				buffer_view< char >( _mapped_view.size(), (char*)mapped );

			size_t stride = out_buffer_view->stride();
			*out_buffer_view =
				buffer_view_base( out_buffer_view->size(), mapped, stride );
		}
	}
}

void vulkan_buffer::unmap()
{
	if ( !_mapped_view.data() )
	{
		LOG_CRITICAL( "map/unmap mismatch" );
	}
	else
	{
		vmaUnmapMemory( _cfg.vk.vma, _vma_allocation );
		_mapped_view = buffer_view_base(
			_mapped_view.size(),
			nullptr,
			_mapped_view.stride() );
	}
}

void vulkan_buffer::reserve( size_t byte_size )
{
	if ( _mapped_view.data() )
	{
		LOG_CRITICAL( "cannot reserve while mapped" );
	}
	else
	{
		vulkan_resource::wait_pending_jobs();

		if ( _mapped_view.size() < byte_size )
		{
			release();
		}

		if ( !_buffer )
		{
			VmaAllocationCreateInfo vma_info = {};
			vma_info.usage = _cfg.vk.vma_usage;
			vma_info.flags = _cfg.vk.vma_flags;

			VkBufferCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.size = byte_size;
			info.usage = _cfg.vk.usage;
			info.sharingMode = _cfg.vk.sharing_mode;

			if ( byte_size >
				 _cfg.vk.device_properties->limits.maxUniformBufferRange )
			{
				info.usage &=
					~( VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT |
					   VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT );
			}

			vmaCreateBuffer(
				_cfg.vk.vma,
				&info,
				&vma_info,
				&_buffer,
				&_vma_allocation,
				nullptr );
			_mapped_view = buffer_view< char >( byte_size, nullptr );

			_mem_metric.add( byte_size );

			vulkan_resource::on_reallocate_gpu_object();
		}
	}
}

void vulkan_buffer::release()
{
	if ( _buffer )
	{
		vmaDestroyBuffer( _cfg.vk.vma, _buffer, _vma_allocation );
	}

	_mem_metric.reset();
	_buffer = nullptr;
	_vma_allocation = nullptr;
	_mapped_view = buffer_view< char >( 0, nullptr );
}

const buffer_view< char >& vulkan_buffer::mapped_view() const
{
	return _mapped_view;
}

vulkan_resource::state& vulkan_buffer::resource_state()
{
	return _resource_state;
}

const vulkan_resource::state& vulkan_buffer::resource_state() const
{
	return _resource_state;
}

void vulkan_buffer::update_descriptor_set(
	VkDescriptorSet descriptor_set,
	const vulkan_parameter::config& parameter_config,
	size_t array_element ) const
{
	VkDescriptorBufferInfo buffer_descriptor{
		_buffer,
		0,
		VK_WHOLE_SIZE,
	};

	VkWriteDescriptorSet write_descriptor = {
		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
		nullptr,
		descriptor_set,
	};

	write_descriptor.dstBinding = (uint32_t)parameter_config.binding;
	write_descriptor.dstArrayElement = (uint32_t)array_element;
	write_descriptor.descriptorCount = (uint32_t)parameter_config.array_size;
	write_descriptor.descriptorType = parameter_config.vk.descriptor_type;
	write_descriptor.pBufferInfo = &buffer_descriptor;

	vkUpdateDescriptorSets( _cfg.vk.device, 1, &write_descriptor, 0, nullptr );
}

void vulkan_buffer::push_barrier(
	uint32_t target_queue_family_index,
	vulkan_barrier_manager* barrier_manager,
	uint32_t src_queue_family_index,
	uint32_t dst_queue_family_index,
	VkImageLayout,
	VkImageLayout,
	const vulkan_job_scope& src_scope,
	const vulkan_job_scope& dst_scope ) const
{
	VkBufferMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
	barrier.pNext = nullptr;
	barrier.srcAccessMask = (VkFlags)src_scope.access;
	barrier.dstAccessMask = (VkFlags)dst_scope.access;
	barrier.srcQueueFamilyIndex = src_queue_family_index;
	barrier.dstQueueFamilyIndex = dst_queue_family_index;
	barrier.buffer = _buffer;
	barrier.offset = 0;
	barrier.size = _mapped_view.byte_size();

	barrier_manager->push_barrier(
		target_queue_family_index,
		src_scope.stages,
		dst_scope.stages,
		barrier );
}