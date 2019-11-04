//
//#include <random>
//#include <numeric>
//#include <fstream>
//#include <iostream>
//#include <chrono>
//#include <string>
//#include <iomanip>
//#include <cassert>
//#include <functional>
//#include <array>
//
//
//#include "vulkan/defines/vulkan_includes.h"
//
//
// namespace vks
//{
//	namespace initializers
//	{
//
//		inline VkMemoryAllocateInfo memory_allocate_info()
//		{
//			VkMemoryAllocateInfo mem_alloc_info{};
//			mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//			return mem_alloc_info;
//		}
//
//		inline VkMappedMemoryRange mapped_memory_range()
//		{
//			VkMappedMemoryRange mapped_memory_range{};
//			mapped_memory_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//			return mapped_memory_range;
//		}
//
//		inline VkCommandBufferAllocateInfo command_buffer_allocate_info(
//			VkCommandPool command_pool,
//			VkCommandBufferLevel level,
//			uint32_t buffer_count)
//		{
//			VkCommandBufferAllocateInfo command_buffer_allocate_info{};
//			command_buffer_allocate_info.sType =
// VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//			command_buffer_allocate_info.commandPool = command_pool;
//			command_buffer_allocate_info.level = level;
//			command_buffer_allocate_info.commandBufferCount = buffer_count;
//			return command_buffer_allocate_info;
//		}
//
//		inline VkCommandPoolCreateInfo command_pool_create_info()
//		{
//			VkCommandPoolCreateInfo cmd_pool_create_info{};
//			cmd_pool_create_info.sType =
// VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO; 			return
// cmd_pool_create_info;
//		}
//
//		inline VkCommandBufferBeginInfo command_buffer_begin_info()
//		{
//			VkCommandBufferBeginInfo cmd_buffer_begin_info{};
//			cmd_buffer_begin_info.sType =
// VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO; 			return
// cmd_buffer_begin_info;
//		}
//
//		inline VkCommandBufferInheritanceInfo command_buffer_inheritance_info()
//		{
//			VkCommandBufferInheritanceInfo cmd_buffer_inheritance_info{};
//			cmd_buffer_inheritance_info.sType =
// VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO; 			return
// cmd_buffer_inheritance_info;
//		}
//
//		inline VkRenderPassBeginInfo render_pass_begin_info()
//		{
//			VkRenderPassBeginInfo render_pass_begin_info{};
//			render_pass_begin_info.sType =
// VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO; 			return
// render_pass_begin_info;
//		}
//
//		inline VkRenderPassCreateInfo render_pass_create_info()
//		{
//			VkRenderPassCreateInfo render_pass_create_info{};
//			render_pass_create_info.sType =
// VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO; 			return
// render_pass_create_info;
//		}
//
//		/** @brief Initialize an image memory barrier with no image transfer
// ownership */ 		inline VkImageMemoryBarrier image_memory_barrier()
//		{
//			VkImageMemoryBarrier image_memory_barrier{};
//			image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//			image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//			image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//			return image_memory_barrier;
//		}
//
//		/** @brief Initialize a buffer memory barrier with no image transfer
// ownership */ 		inline VkBufferMemoryBarrier buffer_memory_barrier()
//		{
//			VkBufferMemoryBarrier buffer_memory_barrier{};
//			buffer_memory_barrier.sType =
// VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
//			buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//			buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//			return buffer_memory_barrier;
//		}
//
//		inline VkMemoryBarrier memory_barrier()
//		{
//			VkMemoryBarrier memory_barrier{};
//			memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
//			return memory_barrier;
//		}
//
//		inline VkImageCreateInfo image_create_info()
//		{
//			VkImageCreateInfo image_create_info{};
//			image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//			return image_create_info;
//		}
//
//		inline VkSamplerCreateInfo sampler_create_info()
//		{
//			VkSamplerCreateInfo sampler_create_info{};
//			sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//			sampler_create_info.maxAnisotropy = 1.0f;
//			return sampler_create_info;
//		}
//
//		inline VkImageViewCreateInfo image_view_create_info()
//		{
//			VkImageViewCreateInfo image_view_create_info{};
//			image_view_create_info.sType =
// VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; 			return
// image_view_create_info;
//		}
//
//		inline VkFramebufferCreateInfo framebuffer_create_info()
//		{
//			VkFramebufferCreateInfo framebuffer_create_info{};
//			framebuffer_create_info.sType =
// VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO; 			return
// framebuffer_create_info;
//		}
//
//		inline VkSemaphoreCreateInfo semaphore_create_info()
//		{
//			VkSemaphoreCreateInfo semaphore_create_info{};
//			semaphore_create_info.sType =
// VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; 			return
// semaphore_create_info;
//		}
//
//		inline VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0)
//		{
//			VkFenceCreateInfo fence_create_info{};
//			fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//			fence_create_info.flags = flags;
//			return fence_create_info;
//		}
//
//		inline VkEventCreateInfo event_create_info()
//		{
//			VkEventCreateInfo event_create_info{};
//			event_create_info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
//			return event_create_info;
//		}
//
//		inline VkSubmitInfo submit_info()
//		{
//			VkSubmitInfo submit_info{};
//			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//			return submit_info;
//		}
//
//		inline VkViewport viewport(
//			float width,
//			float height,
//			float min_depth,
//			float max_depth)
//		{
//			VkViewport viewport{};
//			viewport.width = width;
//			viewport.height = height;
//			viewport.minDepth = min_depth;
//			viewport.maxDepth = max_depth;
//			return viewport;
//		}
//
//		inline VkRect2D rect_2_d(
//			int32_t width,
//			int32_t height,
//			int32_t offset_x,
//			int32_t offset_y)
//		{
//			VkRect2D rect2D{};
//			rect2D.extent.width = width;
//			rect2D.extent.height = height;
//			rect2D.offset.x = offset_x;
//			rect2D.offset.y = offset_y;
//			return rect2D;
//		}
//
//		inline VkBufferCreateInfo buffer_create_info()
//		{
//			VkBufferCreateInfo buf_create_info{};
//			buf_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//			return buf_create_info;
//		}
//
//		inline VkBufferCreateInfo buffer_create_info(
//			VkBufferUsageFlags usage,
//			VkDeviceSize size)
//		{
//			VkBufferCreateInfo buf_create_info{};
//			buf_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//			buf_create_info.usage = usage;
//			buf_create_info.size = size;
//			return buf_create_info;
//		}
//
//		inline VkDescriptorPoolCreateInfo descriptor_pool_create_info(
//			uint32_t pool_size_count,
//			VkDescriptorPoolSize* p_pool_sizes,
//			uint32_t max_sets)
//		{
//			VkDescriptorPoolCreateInfo descriptor_pool_info{};
//			descriptor_pool_info.sType =
// VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//			descriptor_pool_info.poolSizeCount = pool_size_count;
//			descriptor_pool_info.pPoolSizes = p_pool_sizes;
//			descriptor_pool_info.maxSets = max_sets;
//			return descriptor_pool_info;
//		}
//
//		inline VkDescriptorPoolCreateInfo descriptor_pool_create_info(
//			const std::vector<VkDescriptorPoolSize>& pool_sizes,
//			uint32_t max_sets)
//		{
//			VkDescriptorPoolCreateInfo descriptor_pool_info{};
//			descriptor_pool_info.sType =
// VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//			descriptor_pool_info.poolSizeCount =
// static_cast<uint32_t>(pool_sizes.size());
// descriptor_pool_info.pPoolSizes = pool_sizes.data();
// descriptor_pool_info.maxSets = max_sets; return descriptor_pool_info;
//		}
//
//		inline VkDescriptorPoolSize descriptor_pool_size(
//			VkDescriptorType type,
//			uint32_t descriptor_count)
//		{
//			VkDescriptorPoolSize descriptor_pool_size{};
//			descriptor_pool_size.type = type;
//			descriptor_pool_size.descriptorCount = descriptor_count;
//			return descriptor_pool_size;
//		}
//
//		inline VkDescriptorSetLayoutBinding descriptor_set_layout_binding(
//			VkDescriptorType type,
//			VkShaderStageFlags stage_flags,
//			uint32_t binding,
//			uint32_t descriptor_count = 1)
//		{
//			VkDescriptorSetLayoutBinding set_layout_binding{};
//			set_layout_binding.descriptorType = type;
//			set_layout_binding.stageFlags = stage_flags;
//			set_layout_binding.binding = binding;
//			set_layout_binding.descriptorCount = descriptor_count;
//			return set_layout_binding;
//		}
//
//		inline VkDescriptorSetLayoutCreateInfo
// descriptor_set_layout_create_info( 			const
// VkDescriptorSetLayoutBinding* p_bindings, 			uint32_t binding_count)
//		{
//			VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
//			descriptor_set_layout_create_info.sType =
// VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//			descriptor_set_layout_create_info.pBindings = p_bindings;
//			descriptor_set_layout_create_info.bindingCount = binding_count;
//			return descriptor_set_layout_create_info;
//		}
//
//		inline VkDescriptorSetLayoutCreateInfo
// descriptor_set_layout_create_info( 			const
// std::vector<VkDescriptorSetLayoutBinding>& bindings)
//		{
//			VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
//			descriptor_set_layout_create_info.sType =
// VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//			descriptor_set_layout_create_info.pBindings = bindings.data();
//			descriptor_set_layout_create_info.bindingCount =
// static_cast<uint32_t>(bindings.size()); 			return
// descriptor_set_layout_create_info;
//		}
//
//		inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(
//			const VkDescriptorSetLayout* p_set_layouts,
//			uint32_t set_layout_count = 1)
//		{
//			VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
//			pipeline_layout_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//			pipeline_layout_create_info.setLayoutCount = set_layout_count;
//			pipeline_layout_create_info.pSetLayouts = p_set_layouts;
//			return pipeline_layout_create_info;
//		}
//
//		inline VkPipelineLayoutCreateInfo pipeline_layout_create_info(
//			uint32_t set_layout_count = 1)
//		{
//			VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
//			pipeline_layout_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//			pipeline_layout_create_info.setLayoutCount = set_layout_count;
//			return pipeline_layout_create_info;
//		}
//
//		inline VkDescriptorSetAllocateInfo descriptor_set_allocate_info(
//			VkDescriptorPool descriptor_pool,
//			const VkDescriptorSetLayout* p_set_layouts,
//			uint32_t descriptor_set_count)
//		{
//			VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
//			descriptor_set_allocate_info.sType =
// VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//			descriptor_set_allocate_info.descriptorPool = descriptor_pool;
//			descriptor_set_allocate_info.pSetLayouts = p_set_layouts;
//			descriptor_set_allocate_info.descriptorSetCount =
// descriptor_set_count; 			return descriptor_set_allocate_info;
//		}
//
//		inline VkDescriptorImageInfo descriptor_image_info(VkSampler sampler,
// VkImageView image_view, VkImageLayout image_layout)
//		{
//			VkDescriptorImageInfo descriptor_image_info{};
//			descriptor_image_info.sampler = sampler;
//			descriptor_image_info.imageView = image_view;
//			descriptor_image_info.imageLayout = image_layout;
//			return descriptor_image_info;
//		}
//
//		inline VkWriteDescriptorSet write_descriptor_set(
//			VkDescriptorSet dst_set,
//			VkDescriptorType type,
//			uint32_t binding,
//			VkDescriptorBufferInfo* buffer_info,
//			uint32_t descriptor_count = 1)
//		{
//			VkWriteDescriptorSet write_descriptor_set{};
//			write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			write_descriptor_set.dstSet = dst_set;
//			write_descriptor_set.descriptorType = type;
//			write_descriptor_set.dstBinding = binding;
//			write_descriptor_set.pBufferInfo = buffer_info;
//			write_descriptor_set.descriptorCount = descriptor_count;
//			return write_descriptor_set;
//		}
//
//		inline VkWriteDescriptorSet write_descriptor_set(
//			VkDescriptorSet dst_set,
//			VkDescriptorType type,
//			uint32_t binding,
//			VkDescriptorImageInfo *image_info,
//			uint32_t descriptor_count = 1)
//		{
//			VkWriteDescriptorSet write_descriptor_set{};
//			write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			write_descriptor_set.dstSet = dst_set;
//			write_descriptor_set.descriptorType = type;
//			write_descriptor_set.dstBinding = binding;
//			write_descriptor_set.pImageInfo = image_info;
//			write_descriptor_set.descriptorCount = descriptor_count;
//			return write_descriptor_set;
//		}
//
//		inline VkVertexInputBindingDescription vertex_input_binding_description(
//			uint32_t binding,
//			uint32_t stride,
//			VkVertexInputRate input_rate)
//		{
//			VkVertexInputBindingDescription v_input_bind_description{};
//			v_input_bind_description.binding = binding;
//			v_input_bind_description.stride = stride;
//			v_input_bind_description.inputRate = input_rate;
//			return v_input_bind_description;
//		}
//
//		inline VkVertexInputAttributeDescription
// vertex_input_attribute_description( 			uint32_t binding,
// uint32_t location, 			VkFormat format, 			uint32_t offset)
//		{
//			VkVertexInputAttributeDescription v_input_attrib_description{};
//			v_input_attrib_description.location = location;
//			v_input_attrib_description.binding = binding;
//			v_input_attrib_description.format = format;
//			v_input_attrib_description.offset = offset;
//			return v_input_attrib_description;
//		}
//
//		inline VkPipelineVertexInputStateCreateInfo
// pipeline_vertex_input_state_create_info()
//		{
//			VkPipelineVertexInputStateCreateInfo
// pipeline_vertex_input_state_create_info{};
//			pipeline_vertex_input_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO; 			return
// pipeline_vertex_input_state_create_info;
//		}
//
//		inline VkPipelineInputAssemblyStateCreateInfo
// pipeline_input_assembly_state_create_info( 			VkPrimitiveTopology
// topology, 			VkPipelineInputAssemblyStateCreateFlags flags,
// VkBool32
// primitive_restart_enable)
//		{
//			VkPipelineInputAssemblyStateCreateInfo
// pipeline_input_assembly_state_create_info{};
//			pipeline_input_assembly_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//			pipeline_input_assembly_state_create_info.topology = topology;
//			pipeline_input_assembly_state_create_info.flags = flags;
//			pipeline_input_assembly_state_create_info.primitiveRestartEnable =
// primitive_restart_enable; 			return
// pipeline_input_assembly_state_create_info;
//		}
//
//		inline VkPipelineRasterizationStateCreateInfo
// pipeline_rasterization_state_create_info( 			VkPolygonMode
// polygon_mode, 			VkCullModeFlags cull_mode, 			VkFrontFace
// front_face,
//			VkPipelineRasterizationStateCreateFlags flags = 0)
//		{
//			VkPipelineRasterizationStateCreateInfo
// pipeline_rasterization_state_create_info{};
//			pipeline_rasterization_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//			pipeline_rasterization_state_create_info.polygonMode = polygon_mode;
//			pipeline_rasterization_state_create_info.cullMode = cull_mode;
//			pipeline_rasterization_state_create_info.frontFace = front_face;
//			pipeline_rasterization_state_create_info.flags = flags;
//			pipeline_rasterization_state_create_info.depthClampEnable =
// VK_FALSE; 			pipeline_rasterization_state_create_info.lineWidth
// = 1.0f; return pipeline_rasterization_state_create_info;
//		}
//
//		inline VkPipelineColorBlendAttachmentState
// pipeline_color_blend_attachment_state( 			VkColorComponentFlags
// color_write_mask, 			VkBool32 blend_enable)
//		{
//			VkPipelineColorBlendAttachmentState
// pipeline_color_blend_attachment_state{};
//			pipeline_color_blend_attachment_state.colorWriteMask =
// color_write_mask; pipeline_color_blend_attachment_state.blendEnable =
// blend_enable; 			return pipeline_color_blend_attachment_state;
//		}
//
//		inline VkPipelineColorBlendStateCreateInfo
// pipeline_color_blend_state_create_info( 			uint32_t attachment_count,
// const VkPipelineColorBlendAttachmentState * p_attachments)
//		{
//			VkPipelineColorBlendStateCreateInfo
// pipeline_color_blend_state_create_info{};
//			pipeline_color_blend_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//			pipeline_color_blend_state_create_info.attachmentCount =
// attachment_count; pipeline_color_blend_state_create_info.pAttachments =
// p_attachments; 			return pipeline_color_blend_state_create_info;
//		}
//
//		inline VkPipelineDepthStencilStateCreateInfo
// pipeline_depth_stencil_state_create_info( 			VkBool32
// depth_test_enable, VkBool32 depth_write_enable, 			VkCompareOp
// depth_compare_op)
//		{
//			VkPipelineDepthStencilStateCreateInfo
// pipeline_depth_stencil_state_create_info{};
//			pipeline_depth_stencil_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//			pipeline_depth_stencil_state_create_info.depthTestEnable =
// depth_test_enable;
// pipeline_depth_stencil_state_create_info.depthWriteEnable =
// depth_write_enable;
// pipeline_depth_stencil_state_create_info.depthCompareOp = depth_compare_op;
// pipeline_depth_stencil_state_create_info.front =
// pipeline_depth_stencil_state_create_info.back;
//			pipeline_depth_stencil_state_create_info.back.compareOp =
// VK_COMPARE_OP_ALWAYS; 			return
// pipeline_depth_stencil_state_create_info;
//		}
//
//		inline VkPipelineViewportStateCreateInfo
// pipeline_viewport_state_create_info( 			uint32_t viewport_count,
// uint32_t scissor_count, 			VkPipelineViewportStateCreateFlags flags =
// 0)
//		{
//			VkPipelineViewportStateCreateInfo
// pipeline_viewport_state_create_info{};
//			pipeline_viewport_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//			pipeline_viewport_state_create_info.viewportCount = viewport_count;
//			pipeline_viewport_state_create_info.scissorCount = scissor_count;
//			pipeline_viewport_state_create_info.flags = flags;
//			return pipeline_viewport_state_create_info;
//		}
//
//		inline VkPipelineMultisampleStateCreateInfo
// pipeline_multisample_state_create_info( 			VkSampleCountFlagBits
// rasterization_samples, 			VkPipelineMultisampleStateCreateFlags flags
// = 0)
//		{
//			VkPipelineMultisampleStateCreateInfo
// pipeline_multisample_state_create_info{};
//			pipeline_multisample_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//			pipeline_multisample_state_create_info.rasterizationSamples =
// rasterization_samples; 			pipeline_multisample_state_create_info.flags
// = flags; 			return pipeline_multisample_state_create_info;
//		}
//
//		inline VkPipelineDynamicStateCreateInfo
// pipeline_dynamic_state_create_info( 			const VkDynamicState *
// p_dynamic_states, 			uint32_t dynamic_state_count,
//			VkPipelineDynamicStateCreateFlags flags = 0)
//		{
//			VkPipelineDynamicStateCreateInfo
// pipeline_dynamic_state_create_info{};
// pipeline_dynamic_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//			pipeline_dynamic_state_create_info.pDynamicStates =
// p_dynamic_states;
// pipeline_dynamic_state_create_info.dynamicStateCount = dynamic_state_count;
// pipeline_dynamic_state_create_info.flags = flags; 			return
// pipeline_dynamic_state_create_info;
//		}
//
//		inline VkPipelineDynamicStateCreateInfo
// pipeline_dynamic_state_create_info( 			const
// std::vector<VkDynamicState>& p_dynamic_states,
// VkPipelineDynamicStateCreateFlags flags = 0)
//		{
//			VkPipelineDynamicStateCreateInfo
// pipeline_dynamic_state_create_info{};
// pipeline_dynamic_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
//			pipeline_dynamic_state_create_info.pDynamicStates =
// p_dynamic_states.data();
// pipeline_dynamic_state_create_info.dynamicStateCount =
// static_cast<uint32_t>(p_dynamic_states.size());
//			pipeline_dynamic_state_create_info.flags = flags;
//			return pipeline_dynamic_state_create_info;
//		}
//
//		inline VkPipelineTessellationStateCreateInfo
// pipeline_tessellation_state_create_info(uint32_t patch_control_points)
//		{
//			VkPipelineTessellationStateCreateInfo
// pipeline_tessellation_state_create_info{};
//			pipeline_tessellation_state_create_info.sType =
// VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
//			pipeline_tessellation_state_create_info.patchControlPoints =
// patch_control_points; 			return
// pipeline_tessellation_state_create_info;
//		}
//
//		inline VkGraphicsPipelineCreateInfo pipeline_create_info(
//			VkPipelineLayout layout,
//			VkRenderPass render_pass,
//			VkPipelineCreateFlags flags = 0)
//		{
//			VkGraphicsPipelineCreateInfo pipeline_create_info{};
//			pipeline_create_info.sType =
// VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
// pipeline_create_info.layout = layout;
// pipeline_create_info.renderPass = render_pass;
// pipeline_create_info.flags
// =
// flags; 			pipeline_create_info.basePipelineIndex = -1;
//			pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
//			return pipeline_create_info;
//		}
//
//		inline VkGraphicsPipelineCreateInfo pipeline_create_info()
//		{
//			VkGraphicsPipelineCreateInfo pipeline_create_info{};
//			pipeline_create_info.sType =
// VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//			pipeline_create_info.basePipelineIndex = -1;
//			pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
//			return pipeline_create_info;
//		}
//
//		inline VkComputePipelineCreateInfo compute_pipeline_create_info(
//			VkPipelineLayout layout,
//			VkPipelineCreateFlags flags = 0)
//		{
//			VkComputePipelineCreateInfo compute_pipeline_create_info{};
//			compute_pipeline_create_info.sType =
// VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
//			compute_pipeline_create_info.layout = layout;
//			compute_pipeline_create_info.flags = flags;
//			return compute_pipeline_create_info;
//		}
//
//		inline VkPushConstantRange push_constant_range(
//			VkShaderStageFlags stage_flags,
//			uint32_t size,
//			uint32_t offset)
//		{
//			VkPushConstantRange push_constant_range{};
//			push_constant_range.stageFlags = stage_flags;
//			push_constant_range.offset = offset;
//			push_constant_range.size = size;
//			return push_constant_range;
//		}
//
//		inline VkBindSparseInfo bind_sparse_info()
//		{
//			VkBindSparseInfo bind_sparse_info{};
//			bind_sparse_info.sType = VK_STRUCTURE_TYPE_BIND_SPARSE_INFO;
//			return bind_sparse_info;
//		}
//
//		/** @brief Initialize a map entry for a shader specialization constant
//*/ 		inline VkSpecializationMapEntry specialization_map_entry(uint32_t
// constant_i_d, uint32_t offset, size_t size)
//		{
//			VkSpecializationMapEntry specialization_map_entry{};
//			specialization_map_entry.constantID = constant_i_d;
//			specialization_map_entry.offset = offset;
//			specialization_map_entry.size = size;
//			return specialization_map_entry;
//		}
//
//		/** @brief Initialize a specialization constant info structure to pass
// to a shader stage */ 		inline VkSpecializationInfo
// specialization_info(uint32_t map_entry_count, const VkSpecializationMapEntry*
// map_entries, size_t data_size, const void* data)
//		{
//			VkSpecializationInfo specialization_info{};
//			specialization_info.mapEntryCount = map_entry_count;
//			specialization_info.pMapEntries = map_entries;
//			specialization_info.dataSize = data_size;
//			specialization_info.pData = data;
//			return specialization_info;
//		}
//	}
//}
//
// namespace tools
//{
//	bool error_mode_silent = false;
//
//	std::string error_string(VkResult error_code)
//	{
//		switch (error_code)
//		{
//#define STR(r) case VK_ ##r: return #r
//			STR(NOT_READY);
//			STR(TIMEOUT);
//			STR(EVENT_SET);
//			STR(EVENT_RESET);
//			STR(INCOMPLETE);
//			STR(ERROR_OUT_OF_HOST_MEMORY);
//			STR(ERROR_OUT_OF_DEVICE_MEMORY);
//			STR(ERROR_INITIALIZATION_FAILED);
//			STR(ERROR_DEVICE_LOST);
//			STR(ERROR_MEMORY_MAP_FAILED);
//			STR(ERROR_LAYER_NOT_PRESENT);
//			STR(ERROR_EXTENSION_NOT_PRESENT);
//			STR(ERROR_FEATURE_NOT_PRESENT);
//			STR(ERROR_INCOMPATIBLE_DRIVER);
//			STR(ERROR_TOO_MANY_OBJECTS);
//			STR(ERROR_FORMAT_NOT_SUPPORTED);
//			STR(ERROR_SURFACE_LOST_KHR);
//			STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
//			STR(SUBOPTIMAL_KHR);
//			STR(ERROR_OUT_OF_DATE_KHR);
//			STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
//			STR(ERROR_VALIDATION_FAILED_EXT);
//			STR(ERROR_INVALID_SHADER_NV);
//#undef STR
//		default:
//			return "UNKNOWN_ERROR";
//		}
//	}
//
//	std::string physical_device_type_string(VkPhysicalDeviceType type)
//	{
//		switch (type)
//		{
//#define STR(r) case VK_PHYSICAL_DEVICE_TYPE_ ##r: return #r
//			STR(OTHER);
//			STR(INTEGRATED_GPU);
//			STR(DISCRETE_GPU);
//			STR(VIRTUAL_GPU);
//#undef STR
//		default: return "UNKNOWN_DEVICE_TYPE";
//		}
//	}
//
//	VkBool32 get_supported_depth_format(VkPhysicalDevice physical_device,
// VkFormat *depth_format)
//	{
//		// Since all depth formats may be optional, we need to find a suitable
// depth format to use
//		// Start with the highest precision packed format
//		std::vector<VkFormat> depth_formats = {
//			VK_FORMAT_D32_SFLOAT_S8_UINT,
//			VK_FORMAT_D32_SFLOAT,
//			VK_FORMAT_D24_UNORM_S8_UINT,
//			VK_FORMAT_D16_UNORM_S8_UINT,
//			VK_FORMAT_D16_UNORM
//		};
//
//		for (auto& format : depth_formats)
//		{
//			VkFormatProperties format_props;
//			vkGetPhysicalDeviceFormatProperties(physical_device, format,
//&format_props);
//			// Format must support depth stencil attachment for optimal tiling
//			if (format_props.optimalTilingFeatures &
// VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
//			{
//				*depth_format = format;
//				return true;
//			}
//		}
//
//		return false;
//	}
//
//	// Create an image memory barrier for changing the layout of
//	// an image and put it into an active command buffer
//	// See chapter 11.4 "Image Layout" for details
//
//	void set_image_layout(
//		VkCommandBuffer cmdbuffer,
//		VkImage image,
//		VkImageLayout old_image_layout,
//		VkImageLayout new_image_layout,
//		VkImageSubresourceRange subresource_range,
//		VkPipelineStageFlags src_stage_mask =
// VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 		VkPipelineStageFlags dst_stage_mask
// = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
//	{
//		// Create an image barrier object
//		VkImageMemoryBarrier image_memory_barrier =
// vks::initializers::image_memory_barrier(); image_memory_barrier.oldLayout =
// old_image_layout; 		image_memory_barrier.newLayout = new_image_layout;
//		image_memory_barrier.image = image;
//		image_memory_barrier.subresourceRange = subresource_range;
//
//		// Source layouts (old)
//		// Source access mask controls actions that have to be finished on the
// old layout
//		// before it will be transitioned to the new layout
//		switch (old_image_layout)
//		{
//		case VK_IMAGE_LAYOUT_UNDEFINED:
//			// Image layout is undefined (or does not matter)
//			// Only valid as initial layout
//			// No flags required, listed only for completeness
//			image_memory_barrier.srcAccessMask = 0;
//			break;
//
//		case VK_IMAGE_LAYOUT_PREINITIALIZED:
//			// Image is preinitialized
//			// Only valid as initial layout for linear images, preserves memory
// contents
//			// Make sure host writes have been finished
//			image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
//			break;
//
//		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//			// Image is a color attachment
//			// Make sure any writes to the color buffer have been finished
//			image_memory_barrier.srcAccessMask =
// VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; 			break;
//
//		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//			// Image is a depth/stencil attachment
//			// Make sure any writes to the depth/stencil buffer have been
// finished 			image_memory_barrier.srcAccessMask =
// VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 			break;
//
//		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
//			// Image is a transfer source
//			// Make sure any reads from the image have been finished
//			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//			break;
//
//		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
//			// Image is a transfer destination
//			// Make sure any writes to the image have been finished
//			image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			break;
//
//		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//			// Image is read by a shader
//			// Make sure any shader reads from the image have been finished
//			image_memory_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
//			break;
//		default:
//			// Other source layouts aren't handled (yet)
//			break;
//		}
//
//		// Target layouts (new)
//		// Destination access mask controls the dependency for the new image
// layout 		switch (new_image_layout)
//		{
//		case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
//			// Image will be used as a transfer destination
//			// Make sure any writes to the image have been finished
//			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			break;
//
//		case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
//			// Image will be used as a transfer source
//			// Make sure any reads from the image have been finished
//			image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//			break;
//
//		case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
//			// Image will be used as a color attachment
//			// Make sure any writes to the color buffer have been finished
//			image_memory_barrier.dstAccessMask =
// VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; 			break;
//
//		case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
//			// Image layout will be used as a depth/stencil attachment
//			// Make sure any writes to depth/stencil buffer have been finished
//			image_memory_barrier.dstAccessMask =
// image_memory_barrier.dstAccessMask |
// VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; 			break;
//
//		case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
//			// Image will be read in a shader (sampler, input attachment)
//			// Make sure any writes to the image have been finished
//			if (image_memory_barrier.srcAccessMask == 0)
//			{
//				image_memory_barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT |
// VK_ACCESS_TRANSFER_WRITE_BIT;
//			}
//			image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//			break;
//		default:
//			// Other source layouts aren't handled (yet)
//			break;
//		}
//
//		// Put barrier inside setup command buffer
//		vkCmdPipelineBarrier(
//			cmdbuffer,
//			src_stage_mask,
//			dst_stage_mask,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &image_memory_barrier);
//	}
//
//	// Fixed sub resource on first mip level and layer
//	void set_image_layout(
//		VkCommandBuffer cmd_buffer,
//		VkImage image,
//		VkImageAspectFlags aspect_mask,
//		VkImageLayout old_image_layout,
//		VkImageLayout new_image_layout,
//		VkPipelineStageFlags src_stage_mask =
// VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 		VkPipelineStageFlags dst_stage_mask
// = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT)
//	{
//		VkImageSubresourceRange subresource_range = {};
//		subresource_range.aspectMask = aspect_mask;
//		subresource_range.baseMipLevel = 0;
//		subresource_range.levelCount = 1;
//		subresource_range.layerCount = 1;
//		set_image_layout(cmd_buffer, image, old_image_layout, new_image_layout,
// subresource_range, src_stage_mask, dst_stage_mask);
//	}
//
//	void insert_image_memory_barrier(
//		VkCommandBuffer cmdbuffer,
//		VkImage image,
//		VkAccessFlags src_access_mask,
//		VkAccessFlags dst_access_mask,
//		VkImageLayout old_image_layout,
//		VkImageLayout new_image_layout,
//		VkPipelineStageFlags src_stage_mask,
//		VkPipelineStageFlags dst_stage_mask,
//		VkImageSubresourceRange subresource_range)
//	{
//		VkImageMemoryBarrier image_memory_barrier =
// vks::initializers::image_memory_barrier();
// image_memory_barrier.srcAccessMask = src_access_mask;
// image_memory_barrier.dstAccessMask = dst_access_mask;
//		image_memory_barrier.oldLayout = old_image_layout;
//		image_memory_barrier.newLayout = new_image_layout;
//		image_memory_barrier.image = image;
//		image_memory_barrier.subresourceRange = subresource_range;
//
//		vkCmdPipelineBarrier(
//			cmdbuffer,
//			src_stage_mask,
//			dst_stage_mask,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &image_memory_barrier);
//	}
//
//	void exit_fatal(std::string message, int32_t exit_code)
//	{
//		std::cerr << message << "\n";
//		exit(exit_code);
//	}
//
//	void exit_fatal(std::string message, VkResult result_code)
//	{
//		exit_fatal(message, (int32_t)result_code);
//	}
//
//	std::string read_text_file(const char *file_name)
//	{
//		std::string file_content;
//		std::ifstream file_stream(file_name, std::ios::in);
//		if (!file_stream.is_open()) {
//			printf("File %s not found\n", file_name);
//			return "";
//		}
//		std::string line = "";
//		while (!file_stream.eof()) {
//			getline(file_stream, line);
//			file_content.append(line + "\n");
//		}
//		file_stream.close();
//		return file_content;
//	}
//
//	VkShaderModule load_shader(const char *file_name, VkDevice device)
//	{
//		std::ifstream is(file_name, std::ios::binary | std::ios::in |
// std::ios::ate);
//
//		if (is.is_open())
//		{
//			size_t size = (size_t)is.tellg();
//			is.seekg(0, std::ios::beg);
//			char* shader_code = new char[size];
//			is.read(shader_code, size);
//			is.close();
//
//			assert(size > 0);
//
//			VkShaderModule shader_module;
//			VkShaderModuleCreateInfo module_create_info{};
//			module_create_info.sType =
// VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
// module_create_info.codeSize = size; 			module_create_info.pCode =
// (uint32_t*)shader_code;
//
//			vkCreateShaderModule(device, &module_create_info, NULL,
//&shader_module);
//
//			delete[] shader_code;
//
//			return shader_module;
//		}
//		else
//		{
//			std::cerr << "Error: Could not open shader file \"" << file_name <<
//"\""
//<< std::endl; 			return VK_NULL_HANDLE;
//		}
//	}
//
//	VkShaderModule load_shader_g_l_s_l(const char *file_name, VkDevice device,
// VkShaderStageFlagBits stage)
//	{
//		std::string shader_src = read_text_file(file_name);
//		const char *shader_code = shader_src.c_str();
//		size_t size = strlen(shader_code);
//		assert(size > 0);
//
//		VkShaderModule shader_module;
//		VkShaderModuleCreateInfo module_create_info;
//		module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//		module_create_info.pNext = NULL;
//		module_create_info.codeSize = 3 * sizeof(uint32_t) + size + 1;
//		module_create_info.pCode =
//(uint32_t*)malloc(module_create_info.codeSize); 		module_create_info.flags
//= 0;
//
//		// Magic SPV number
//		((uint32_t *)module_create_info.pCode)[0] = 0x07230203;
//		((uint32_t *)module_create_info.pCode)[1] = 0;
//		((uint32_t *)module_create_info.pCode)[2] = stage;
//		memcpy(((uint32_t *)module_create_info.pCode + 3), shader_code, size +
// 1);
//
//		vkCreateShaderModule(device, &module_create_info, NULL, &shader_module);
//
//		return shader_module;
//	}
//
//	bool file_exists(const std::string &file_name)
//	{
//		std::ifstream f(file_name.c_str());
//		return !f.fail();
//	}
//}
//
// class Camera
//{
// private:
//	float _fov;
//	float _znear, _zfar;
//
//	void update_view_matrix()
//	{
//		glm::mat4 rot_m = glm::mat4(1.0f);
//		glm::mat4 trans_m;
//
//		rotM = glm::rotate(rotM, glm::radians(_rotation.x), glm::vec3(1.0f,
// 0.0f, 0.0f)); 		rotM = glm::rotate(rotM, glm::radians(_rotation.y),
// glm::vec3(0.0f, 1.0f, 0.0f)); 		rotM = glm::rotate(rotM,
// glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
//
//		transM = glm::translate(glm::mat4(1.0f), _position);
//
//		if (_type == CameraType::firstperson)
//		{
//			_matrices._view = rot_m * trans_m;
//		}
//		else
//		{
//			_matrices._view = trans_m * rot_m;
//		}
//
//		_updated = true;
//	};
// public:
//	enum CameraType { lookat, firstperson, };
//	CameraType _type = CameraType::lookat;
//
//	glm::vec3 _rotation = glm::vec3();
//	glm::vec3 _position = glm::vec3();
//
//	float _rotation_speed = 1.0f;
//	float _movement_speed = 1.0f;
//
//	bool _updated = false;
//
//	struct
//	{
//		glm::mat4 _perspective;
//		glm::mat4 _view;
//	} _matrices;
//
//	struct
//	{
//		bool _left = false;
//		bool _right = false;
//		bool _up = false;
//		bool _down = false;
//	} _keys;
//
//	bool moving()
//	{
//		return _keys._left || _keys._right || _keys._up || _keys._down;
//	}
//
//	float get_near_clip() {
//		return _znear;
//	}
//
//	float get_far_clip() {
//		return _zfar;
//	}
//
//	void set_perspective(float fov, float aspect, float znear, float zfar)
//	{
//		_fov = fov;
//		_znear = znear;
//		_zfar = zfar;
//		_matrices._perspective = glm::perspective(glm::radians(fov), aspect,
// znear, zfar);
//	};
//
//	void update_aspect_ratio(float aspect)
//	{
//		_matrices._perspective = glm::perspective(glm::radians(_fov), aspect,
//_znear, _zfar);
//	}
//
//	void set_position(glm::vec3 position)
//	{
//		_position = position;
//		updateViewMatrix();
//	}
//
//	void set_rotation(glm::vec3 rotation)
//	{
//		_rotation = rotation;
//		updateViewMatrix();
//	};
//
//	void rotate(glm::vec3 delta)
//	{
//		_rotation += delta;
//		updateViewMatrix();
//	}
//
//	void set_translation(glm::vec3 translation)
//	{
//		_position = translation;
//		updateViewMatrix();
//	};
//
//	void translate(glm::vec3 delta)
//	{
//		_position += delta;
//		updateViewMatrix();
//	}
//
//	void update(float delta_time)
//	{
//		_updated = false;
//		if (_type == CameraType::firstperson)
//		{
//			if (moving())
//			{
//				glm::vec3 cam_front;
//				camFront.x = -cos(glm::radians(_rotation.x)) *
// sin(glm::radians(_rotation.y)); 				camFront.y =
// sin(glm::radians(_rotation.x)); 				camFront.z =
// cos(glm::radians(_rotation.x)) * cos(glm::radians(_rotation.y));
// camFront = glm::normalize(camFront);
//
//				float move_speed = delta_time * _movement_speed;
//
//				if (_keys._up)
//					_position += cam_front * move_speed;
//				if (_keys._down)
//					_position -= cam_front * move_speed;
//				if (_keys._left)
//					_position -= glm::normalize(glm::cross(camFront,
// glm::vec3(0.0f, 1.0f, 0.0f))) * move_speed; 				if (_keys._right)
// _position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f,
// 0.0f)))
// * move_speed;
//
//				updateViewMatrix();
//			}
//		}
//	};
//};
//
// namespace vks
//{
//	class Benchmark {
//	private:
//		FILE *_stream;
//		VkPhysicalDeviceProperties _device_props;
//	public:
//		bool _active = false;
//		bool _output_frame_times = false;
//		uint32_t _warmup = 1;
//		uint32_t _duration = 10;
//		std::vector<double> _frame_times;
//		std::string _file_name = "";
//
//		double _runtime = 0.0;
//		uint32_t _frame_count = 0;
//
//		void run(std::function<void()> render_func, VkPhysicalDeviceProperties
// device_props) { 			_active = true; 			_device_props =
// device_props; std::cout << std::fixed << std::setprecision(3);
//
//			// Warm up phase to get more stable frame rates
//			{
//				double t_measured = 0.0;
//				while (tMeasured < (_warmup * 1000)) {
//					auto t_start = std::chrono::high_resolution_clock::now();
//					renderFunc();
//					auto t_diff = std::chrono::duration<double,
// std::milli>(std::chrono::high_resolution_clock::now() - t_start).count();
//					tMeasured += t_diff;
//				};
//			}
//
//			// Benchmark phase
//			{
//				while (_runtime < (_duration * 1000.0)) {
//					auto t_start = std::chrono::high_resolution_clock::now();
//					renderFunc();
//					auto t_diff = std::chrono::duration<double,
// std::milli>(std::chrono::high_resolution_clock::now() - t_start).count();
//					_runtime += t_diff;
//					_frame_times.push_back(tDiff);
//					_frame_count++;
//				};
//				std::cout << "Benchmark finished" << std::endl;
//				std::cout << "device : " << device_props.deviceName << " (driver
// version: " << device_props.driverVersion << ")" << std::endl;
// std::cout
// << "runtime: " << (_runtime / 1000.0) << std::endl; 				std::cout <<
//"frames : " <<
//_frame_count << std::endl; 				std::cout << "fps    : " <<
//_frame_count
///
//(_runtime / 1000.0) << std::endl;
//			}
//		}
//
//		void save_results() {
//			std::ofstream result(_file_name, std::ios::out);
//			if (result.is_open()) {
//				result << std::fixed << std::setprecision(4);
//
//				result << "device,driverversion,duration (ms),frames,fps" <<
// std::endl; 				result << _device_props.deviceName << "," <<
//_device_props.driverVersion << "," << _runtime << "," << _frame_count << ","
//<< _frame_count / (_runtime / 1000.0) << std::endl;
//
//				if (_output_frame_times) {
//					result << std::endl << "frame,ms" << std::endl;
//					for (size_t i = 0; i < _frame_times.size(); i++) {
//						result << i << "," << _frame_times[i] << std::endl;
//					}
//					double t_min = *std::min_element(_frame_times.begin(),
//_frame_times.end()); 					double t_max =
//*std::max_element(_frame_times.begin(),
//_frame_times.end()); 					double t_avg =
// std::accumulate(_frame_times.begin(), _frame_times.end(), 0.0) /
//(double)_frame_times.size(); 					std::cout << "best   : " <<
//(1000.0 / t_min) << " fps (" << t_min << " ms)" << std::endl;
// std::cout
//<< "worst  : " << (1000.0 / t_max) << " fps (" << t_max << " ms)" <<
// std::endl; 					std::cout << "avg    : " << (1000.0 / t_avg) << " fps
// ("
// << t_avg
//<< " ms)" << std::endl; 					std::cout << std::endl;
//				}
//
//				result.flush();
//			}
//		}
//	};
//}
//
///*
//* Class wrapping access to the swap chain
//*
//* A swap chain is a collection of framebuffers used for rendering and
// presentation to the windowing system
//*
//* Copyright (C) 2016-2017 by Sascha Willems - www.saschawillems.de
//*
//* This code is licensed under the MIT license (MIT)
//(http://opensource.org/licenses/MIT)
//*/
//
//
//// Custom define for better code readability
//#define VK_FLAGS_NONE 0
//// Default fence timeout in nanoseconds
//#define DEFAULT_FENCE_TIMEOUT 100000000000
//
//#if defined(__ANDROID__)
//#define ASSET_PATH ""
//#else
//#define ASSET_PATH "./../data/"
//#endif
//
// typedef struct _SwapChainBuffers {
//	VkImage _image;
//	VkImageView _view;
//} SwapChainBuffer;
//
//
// class VulkanSwapChain
//{
// private:
//	VkInstance _instance;
//	VkDevice _device;
//	VkPhysicalDevice _physical_device;
//	VkSurfaceKHR _surface;
//
// public:
//	VkFormat _color_format;
//	VkColorSpaceKHR _color_space;
//	/** @brief Handle to the current swap chain, required for recreation */
//	VkSwapchainKHR _swap_chain = VK_NULL_HANDLE;
//	uint32_t _image_count;
//	std::vector<VkImage> _images;
//	std::vector<SwapChainBuffer> _buffers;
//	/** @brief Queue family index of the detected graphics and presenting device
// queue */ 	uint32_t _queue_node_index = UINT32_MAX;
//
//	/** @brief Creates the platform specific surface abstraction of the native
// platform window used for presentation */ 	void init_surface(void*
// platform_handle, void* platform_window)
//	{
//		VkResult err = VK_SUCCESS;
//
//		// Create the os-specific surface
//		VkWin32SurfaceCreateInfoKHR surface_create_info = {};
//		surfaceCreateInfo.sType =
// VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
// surfaceCreateInfo.hinstance = (HINSTANCE)platformHandle;
// surfaceCreateInfo.hwnd = (HWND)platformWindow; 		err =
// vk_create_win_32_surface_k_h_r(_instance, &surfaceCreateInfo, nullptr,
//&_surface);
//
//		if (err != VK_SUCCESS) {
//			tools::exitFatal("Could not create surface!", err);
//		}
//
//		// Get available queue family properties
//		uint32_t queue_count;
//		vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queueCount,
// NULL); 		assert(queueCount >= 1);
//
//		std::vector<VkQueueFamilyProperties> queue_props(queueCount);
//		vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &queueCount,
// queue_props.data());
//
//		// Iterate over each queue to learn whether it supports presenting:
//		// Find a queue with present support
//		// Will be used to present the swap chain images to the windowing system
//		std::vector<VkBool32> supports_present(queueCount);
//		for (uint32_t i = 0; i < queue_count; i++)
//		{
//			vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, i, _surface,
//&supportsPresent[i]);
//		}
//
//		// Search for a graphics and a present queue in the array of queue
//		// families, try to find one that supports both
//		uint32_t graphics_queue_node_index = UINT32_MAX;
//		uint32_t present_queue_node_index = UINT32_MAX;
//		for (uint32_t i = 0; i < queue_count; i++)
//		{
//			if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
//			{
//				if (graphicsQueueNodeIndex == UINT32_MAX)
//				{
//					graphicsQueueNodeIndex = i;
//				}
//
//				if (supportsPresent[i] == VK_TRUE)
//				{
//					graphicsQueueNodeIndex = i;
//					presentQueueNodeIndex = i;
//					break;
//				}
//			}
//		}
//		if (presentQueueNodeIndex == UINT32_MAX)
//		{
//			// If there's no queue that supports both present and graphics
//			// try to find a separate present queue
//			for (uint32_t i = 0; i < queue_count; ++i)
//			{
//				if (supportsPresent[i] == VK_TRUE)
//				{
//					presentQueueNodeIndex = i;
//					break;
//				}
//			}
//		}
//
//		// Exit if either a graphics or a presenting queue hasn't been found
//		if (graphicsQueueNodeIndex == UINT32_MAX || present_queue_node_index ==
// UINT32_MAX)
//		{
//			tools::exitFatal("Could not find a graphics and/or presenting
// queue!", -1);
//		}
//
//		// todo : Add support for separate graphics and presenting queue
//		if (graphicsQueueNodeIndex != present_queue_node_index)
//		{
//			tools::exitFatal("Separate graphics and presenting queues are not
// supported yet!", -1);
//		}
//
//		_queue_node_index = graphics_queue_node_index;
//
//		// Get list of supported surface formats
//		uint32_t format_count;
//		vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface,
//&formatCount, NULL); 		assert(formatCount > 0);
//
//		std::vector<VkSurfaceFormatKHR> surface_formats(formatCount);
//		vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, _surface,
//&formatCount, surface_formats.data());
//
//		// If the surface format list only includes one entry with
// VK_FORMAT_UNDEFINED,
//		// there is no preferered format, so we assume VK_FORMAT_B8G8R8A8_UNORM
//		if ((formatCount == 1) && (surfaceFormats[0].format ==
// VK_FORMAT_UNDEFINED))
//		{
//			_color_format = VK_FORMAT_B8G8R8A8_UNORM;
//			_color_space = surface_formats[0].colorSpace;
//		}
//		else
//		{
//			// iterate over the list of available surface format and
//			// check for the presence of VK_FORMAT_B8G8R8A8_UNORM
//			bool found_B8G8R8A8_UNORM = false;
//			for (auto&& surface_format : surface_formats)
//			{
//				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
//				{
//					_color_format = surface_format.format;
//					_color_space = surface_format.colorSpace;
//					found_B8G8R8A8_UNORM = true;
//					break;
//				}
//			}
//
//			// in case VK_FORMAT_B8G8R8A8_UNORM is not available
//			// select the first available color format
//			if (!found_B8G8R8A8_UNORM)
//			{
//				_color_format = surface_formats[0].format;
//				_color_space = surface_formats[0].colorSpace;
//			}
//		}
//
//	}
//
//	/**
//	* Set instance, physical and logical device to use for the swapchain and get
// all required function pointers
//	*
//	* @param instance Vulkan instance to use
//	* @param physical_device Physical device used to query properties and
// formats relevant to the swapchain
//	* @param device Logical representation of the device to create the swapchain
// for
//	*
//	*/
//	void connect(VkInstance instance, VkPhysicalDevice physical_device, VkDevice
// device)
//	{
//		_instance = instance;
//		_physical_device = physical_device;
//		_device = device;
//	}
//
//	/**
//	* Create the swapchain and get it's images with given width and height
//	*
//	* @param width Pointer to the width of the swapchain (may be adjusted to fit
// the requirements of the swapchain)
//	* @param height Pointer to the height of the swapchain (may be adjusted to
// fit the requirements of the swapchain)
//	* @param vsync (Optional) Can be used to force vsync'd rendering (by using
// VK_PRESENT_MODE_FIFO_KHR as presentation mode)
//	*/
//	void create(uint32_t *width, uint_32_t *height, bool vsync = false)
//	{
//		VkSwapchainKHR old_swapchain = _swap_chain;
//
//		// Get physical device surface properties and formats
//		VkSurfaceCapabilitiesKHR surf_caps;
//		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, _surface,
//&surfCaps);
//
//		// Get available present modes
//		uint32_t present_mode_count;
//		vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, _surface,
//&presentModeCount, NULL); 		assert(presentModeCount > 0);
//
//		std::vector<VkPresentModeKHR> present_modes(presentModeCount);
//		vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, _surface,
//&presentModeCount, present_modes.data());
//
//		VkExtent2D swapchain_extent = {};
//		// If width (and height) equals the special value 0xFFFFFFFF, the size
// of the surface will be set by the swapchain 		if
// (surfCaps.currentExtent.width == (uint32_t)-1)
//		{
//			// If the surface size is undefined, the size is set to
//			// the size of the images requested.
//			swapchainExtent.width = *width;
//			swapchainExtent.height = *height;
//		}
//		else
//		{
//			// If the surface size is defined, the swap chain size must match
//			swapchainExtent = surf_caps.currentExtent;
//			*width = surf_caps.currentExtent.width;
//			*height = surf_caps.currentExtent.height;
//		}
//
//
//		// Select a present mode for the swapchain
//
//		// The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
//		// This mode waits for the vertical blank ("v-sync")
//		VkPresentModeKHR swapchain_present_mode = VK_PRESENT_MODE_FIFO_KHR;
//
//		// If v-sync is not requested, try to find a mailbox mode
//		// It's the lowest latency non-tearing present mode available
//		if (!vsync)
//		{
//			for (size_t i = 0; i < present_mode_count; i++)
//			{
//				if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
//				{
//					swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
//					break;
//				}
//				if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
//(presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
//				{
//					swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
//				}
//			}
//		}
//
//		// Determine the number of images
//		uint32_t desired_number_of_swapchain_images = surf_caps.minImageCount +
// 1; 		if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages
// > surf_caps.maxImageCount))
//		{
//			desiredNumberOfSwapchainImages = surf_caps.maxImageCount;
//		}
//
//		// Find the transformation of the surface
//		VkSurfaceTransformFlagsKHR pre_transform;
//		if (surfCaps.supportedTransforms &
// VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
//		{
//			// We prefer a non-rotated transform
//			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
//		}
//		else
//		{
//			preTransform = surf_caps.currentTransform;
//		}
//
//		// Find a supported composite alpha format (not all devices support
// alpha opaque) 		VkCompositeAlphaFlagBitsKHR composite_alpha =
// VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//		// Simply select the first composite alpha format available
//		std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags = {
//			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
//			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
//			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
//			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
//		};
//		for (auto& composite_alpha_flag : composite_alpha_flags) {
//			if (surfCaps.supportedCompositeAlpha & composite_alpha_flag) {
//				compositeAlpha = composite_alpha_flag;
//				break;
//			};
//		}
//
//		VkSwapchainCreateInfoKHR swapchain_c_i = {};
//		swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//		swapchainCI.pNext = NULL;
//		swapchainCI.surface = _surface;
//		swapchainCI.minImageCount = desired_number_of_swapchain_images;
//		swapchainCI.imageFormat = _color_format;
//		swapchainCI.imageColorSpace = _color_space;
//		swapchainCI.imageExtent = { swapchain_extent.width,
// swapchain_extent.height }; 		swapchainCI.imageUsage =
// VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; 		swapchainCI.preTransform =
//(VkSurfaceTransformFlagBitsKHR)preTransform; 		swapchainCI.imageArrayLayers
//= 1; 		swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		swapchainCI.queueFamilyIndexCount = 0;
//		swapchainCI.pQueueFamilyIndices = NULL;
//		swapchainCI.presentMode = swapchain_present_mode;
//		swapchainCI.oldSwapchain = old_swapchain;
//		// Setting clipped to VK_TRUE allows the implementation to discard
// rendering outside of the surface area 		swapchainCI.clipped = VK_TRUE;
//		swapchainCI.compositeAlpha = composite_alpha;
//
//		// Enable transfer source on swap chain images if supported
//		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
//			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
//		}
//
//		// Enable transfer destination on swap chain images if supported
//		if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
//			swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//		}
//
//		vkCreateSwapchainKHR(_device, &swapchainCI, nullptr, &_swap_chain);
//
//		// If an existing swap chain is re-created, destroy the old swap chain
//		// This also cleans up all the presentable images
//		if (oldSwapchain != VK_NULL_HANDLE)
//		{
//			for (uint32_t i = 0; i < _image_count; i++)
//			{
//				vkDestroyImageView(_device, _buffers[i]._view, nullptr);
//			}
//			vkDestroySwapchainKHR(_device, old_swapchain, nullptr);
//		}
//		vkGetSwapchainImagesKHR(_device, _swap_chain, &_image_count, NULL);
//
//		// Get the swap chain images
//		_images.resize(_image_count);
//		vkGetSwapchainImagesKHR(_device, _swap_chain, &_image_count,
//_images.data());
//
//		// Get the swap chain buffers containing the image and imageview
//		_buffers.resize(_image_count);
//		for (uint32_t i = 0; i < _image_count; i++)
//		{
//			VkImageViewCreateInfo color_attachment_view = {};
//			colorAttachmentView.sType =
// VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; colorAttachmentView.pNext = NULL;
// colorAttachmentView.format = _color_format;
//			colorAttachmentView.components = {
//				VK_COMPONENT_SWIZZLE_R,
//				VK_COMPONENT_SWIZZLE_G,
//				VK_COMPONENT_SWIZZLE_B,
//				VK_COMPONENT_SWIZZLE_A
//			};
//			colorAttachmentView.subresourceRange.aspectMask =
// VK_IMAGE_ASPECT_COLOR_BIT;
// colorAttachmentView.subresourceRange.baseMipLevel = 0;
// colorAttachmentView.subresourceRange.levelCount = 1;
//			colorAttachmentView.subresourceRange.baseArrayLayer = 0;
//			colorAttachmentView.subresourceRange.layerCount = 1;
//			colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
//			colorAttachmentView.flags = 0;
//
//			_buffers[i]._image = _images[i];
//
//			colorAttachmentView.image = _buffers[i]._image;
//
//			vkCreateImageView(_device, &colorAttachmentView, nullptr,
//&_buffers[i]._view);
//		}
//	}
//
//	/**
//	* Acquires the next image in the swap chain
//	*
//	* @param present_complete_semaphore (Optional) Semaphore that is signaled
// when the image is ready for use
//	* @param image_index Pointer to the image index that will be increased if
// the next image could be acquired
//	*
//	* @note The function will always wait until the next image has been acquired
// by setting timeout to UINT64_MAX
//	*
//	* @return VkResult of the image acquisition
//	*/
//	VkResult acquire_next_image(VkSemaphore present_complete_semaphore,
// uint_32_t *imageIndex)
//	{
//		// By setting timeout to UINT64_MAX we will always wait until the next
// image has been acquired or an actual error is thrown
//		// With that we don't have to handle VK_NOT_READY
//		return vk_acquire_next_image_k_h_r(_device, _swap_chain, UINT64_MAX,
// present_complete_semaphore, (VkFence)nullptr, image_index);
//	}
//
//	/**
//	* Queue an image for presentation
//	*
//	* @param queue Presentation queue for presenting the image
//	* @param image_index Index of the swapchain image to queue for presentation
//	* @param wait_semaphore (Optional) Semaphore that is waited on before the
// image is presented (only used if != VK_NULL_HANDLE)
//	*
//	* @return VkResult of the queue presentation
//	*/
//	VkResult queue_present(VkQueue queue, uint_32_t image_index, VkSemaphore
// wait_semaphore = VK_NULL_HANDLE)
//	{
//		VkPresentInfoKHR present_info = {};
//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//		presentInfo.pNext = NULL;
//		presentInfo.swapchainCount = 1;
//		presentInfo.pSwapchains = &_swap_chain;
//		presentInfo.pImageIndices = &imageIndex;
//		// Check if a wait semaphore has been specified to wait for before
// presenting the image 		if (waitSemaphore != VK_NULL_HANDLE)
//		{
//			presentInfo.pWaitSemaphores = &waitSemaphore;
//			presentInfo.waitSemaphoreCount = 1;
//		}
//		return vk_queue_present_k_h_r(queue, &presentInfo);
//	}
//
//
//	/**
//	* Destroy and free Vulkan resources used for the swapchain
//	*/
//	void cleanup()
//	{
//		if (_swap_chain != VK_NULL_HANDLE)
//		{
//			for (uint32_t i = 0; i < _image_count; i++)
//			{
//				vkDestroyImageView(_device, _buffers[i]._view, nullptr);
//			}
//		}
//		if (_surface != VK_NULL_HANDLE)
//		{
//			vkDestroySwapchainKHR(_device, _swap_chain, nullptr);
//			vkDestroySurfaceKHR(_instance, _surface, nullptr);
//		}
//		_surface = VK_NULL_HANDLE;
//		_swap_chain = VK_NULL_HANDLE;
//	}
//};
//
// namespace vks
//{
//	/**
//	* @brief Encapsulates access to a Vulkan buffer backed up by device memory
//	* @note To be filled by an external source like the VulkanDevice
//	*/
//	struct Buffer
//	{
//		VkDevice _device;
//		VkBuffer _buffer = VK_NULL_HANDLE;
//		VkDeviceMemory _memory = VK_NULL_HANDLE;
//		VkDescriptorBufferInfo _descriptor;
//		VkDeviceSize _size = 0;
//		VkDeviceSize _alignment = 0;
//		void* _mapped = nullptr;
//
//		/** @brief Usage flags to be filled by external source at buffer
// creation (to query at some later point) */ 		VkBufferUsageFlags
//_usage_flags;
//		/** @brief Memory propertys flags to be filled by external source at
// buffer creation (to query at some later point) */ VkMemoryPropertyFlags
//_memory_property_flags;
//
//		/**
//		* Map a memory range of this buffer. If successful, mapped points to the
// specified buffer range.
//		*
//		* @param size (Optional) Size of the memory range to map. Pass
// VK_WHOLE_SIZE to map the complete buffer range.
//		* @param offset (Optional) Byte offset from beginning
//		*
//		* @return VkResult of the buffer mapping call
//		*/
//		VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
//		{
//			return vk_map_memory(_device, _memory, offset, size, 0, &_mapped);
//		}
//
//		/**
//		* Unmap a mapped memory range
//		*
//		* @note Does not return a result as vk_unmap_memory can't fail
//		*/
//		void unmap()
//		{
//			if (_mapped)
//			{
//				vkUnmapMemory(_device, _memory);
//				_mapped = nullptr;
//			}
//		}
//
//		/**
//		* Attach the allocated memory block to the buffer
//		*
//		* @param offset (Optional) Byte offset (from the beginning) for the
// memory region to bind
//		*
//		* @return VkResult of the bind_buffer_memory call
//		*/
//		VkResult bind(VkDeviceSize offset = 0)
//		{
//			return vk_bind_buffer_memory(_device, _buffer, _memory, offset);
//		}
//
//		/**
//		* Setup the default descriptor for this buffer
//		*
//		* @param size (Optional) Size of the memory range of the descriptor
//		* @param offset (Optional) Byte offset from beginning
//		*
//		*/
//		void setup_descriptor(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize
// offset = 0)
//		{
//			_descriptor.offset = offset;
//			_descriptor.buffer = _buffer;
//			_descriptor.range = size;
//		}
//
//		/**
//		* Copies the specified data to the mapped buffer
//		*
//		* @param data Pointer to the data to copy
//		* @param size Size of the data to copy in machine units
//		*
//		*/
//		void copy_to(void* data, VkDeviceSize size)
//		{
//			assert(_mapped);
//			memcpy(_mapped, data, (size_t)size);
//		}
//
//		/**
//		* Flush a memory range of the buffer to make it visible to the device
//		*
//		* @note Only required for non-coherent memory
//		*
//		* @param size (Optional) Size of the memory range to flush. Pass
// VK_WHOLE_SIZE to flush the complete buffer range.
//		* @param offset (Optional) Byte offset from beginning
//		*
//		* @return VkResult of the flush call
//		*/
//		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset =
// 0)
//		{
//			VkMappedMemoryRange mapped_range = {};
//			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//			mappedRange.memory = _memory;
//			mappedRange.offset = offset;
//			mappedRange.size = size;
//			return vk_flush_mapped_memory_ranges(_device, 1, &mappedRange);
//		}
//
//		/**
//		* Invalidate a memory range of the buffer to make it visible to the host
//		*
//		* @note Only required for non-coherent memory
//		*
//		* @param size (Optional) Size of the memory range to invalidate. Pass
// VK_WHOLE_SIZE to invalidate the complete buffer range.
//		* @param offset (Optional) Byte offset from beginning
//		*
//		* @return VkResult of the invalidate call
//		*/
//		VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize
// offset = 0)
//		{
//			VkMappedMemoryRange mapped_range = {};
//			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
//			mappedRange.memory = _memory;
//			mappedRange.offset = offset;
//			mappedRange.size = size;
//			return vk_invalidate_mapped_memory_ranges(_device, 1, &mappedRange);
//		}
//
//		/**
//		* Release all Vulkan resources held by this buffer
//		*/
//		void destroy()
//		{
//			if (_buffer)
//			{
//				vkDestroyBuffer(_device, _buffer, nullptr);
//			}
//			if (_memory)
//			{
//				vkFreeMemory(_device, _memory, nullptr);
//			}
//		}
//
//	};
//}
//
// namespace vks
//{
//	struct VulkanDevice
//	{
//		/** @brief Physical device representation */
//		VkPhysicalDevice _physical_device;
//		/** @brief Logical device representation (application's view of the
// device) */ 		VkDevice _logical_device;
//		/** @brief Properties of the physical device including limits that the
// application can check against */ 		VkPhysicalDeviceProperties
// _properties;
//		/** @brief Features of the physical device that an application can use
// to check if a feature is supported */ 		VkPhysicalDeviceFeatures
// _features;
//		/** @brief Features that have been enabled for use on the physical
// device
//*/ 		VkPhysicalDeviceFeatures _enabled_features;
//		/** @brief Memory types and heaps of the physical device */
//		VkPhysicalDeviceMemoryProperties _memory_properties;
//		/** @brief Queue family properties of the physical device */
//		std::vector<VkQueueFamilyProperties> _queue_family_properties;
//		/** @brief List of extensions supported by the device */
//		std::vector<std::string> _supported_extensions;
//
//		/** @brief Default command pool for the graphics queue family index */
//		VkCommandPool _command_pool = VK_NULL_HANDLE;
//
//		/** @brief Contains queue family indices */
//		struct
//		{
//			uint32_t _graphics;
//			uint32_t _compute;
//			uint32_t _transfer;
//		} _queue_family_indices;
//
//		/**  @brief Typecast to VkDevice */
//		operator VkDevice() { return _logical_device; };
//
//		/**
//		* Default constructor
//		*
//		* @param physical_device Physical device that is to be used
//		*/
//		VulkanDevice(VkPhysicalDevice physical_device)
//		{
//			assert(physicalDevice);
//			_physical_device = physical_device;
//
//			// Store Properties features, limits and properties of the physical
// device for later use
//			// Device properties also contain limits and sparse properties
//			vkGetPhysicalDeviceProperties(physicalDevice, &_properties);
//			// Features should be checked by the examples before using them
//			vkGetPhysicalDeviceFeatures(physicalDevice, &_features);
//			// Memory properties are used regularly for creating all kinds of
// buffers 			vkGetPhysicalDeviceMemoryProperties(physicalDevice,
//&_memory_properties);
//			// Queue family properties, used for setting up requested queues
// upon device creation 			uint32_t queue_family_count;
//			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
//&queueFamilyCount, nullptr); 			assert(queueFamilyCount > 0);
//			_queue_family_properties.resize(queueFamilyCount);
//			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice,
//&queueFamilyCount, _queue_family_properties.data());
//
//			// Get list of supported extensions
//			uint32_t ext_count = 0;
//			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr,
//&extCount, nullptr); 			if (extCount > 0)
//			{
//				std::vector<VkExtensionProperties> extensions(extCount);
//				if (vkEnumerateDeviceExtensionProperties(physicalDevice,
// nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
//				{
//					for (auto ext : extensions)
//					{
//						_supported_extensions.push_back(ext.extensionName);
//					}
//				}
//			}
//		}
//
//		/**
//		* Default destructor
//		*
//		* @note Frees the logical device
//		*/
//		~VulkanDevice()
//		{
//			if (_command_pool)
//			{
//				vkDestroyCommandPool(_logical_device, _command_pool, nullptr);
//			}
//			if (_logical_device)
//			{
//				vkDestroyDevice(_logical_device, nullptr);
//			}
//		}
//
//		/**
//		* Get the index of a memory type that has all the requested property
// bits set
//		*
//		* @param type_bits Bitmask with bits set for each memory type supported
// by the resource to request for (from VkMemoryRequirements)
//		* @param properties Bitmask of properties for the memory type to request
//		* @param (Optional) mem_type_found Pointer to a bool that is set to true
// if a matching memory type has been found
//		*
//		* @return Index of the requested memory type
//		*
//		* @throw Throws an exception if mem_type_found is null and no memory
// type could be found that supports the requested properties
//		*/
//		uint32_t get_memory_type(uint32_t type_bits, VkMemoryPropertyFlags
// properties, VkBool32 *memTypeFound = nullptr)
//		{
//			for (uint32_t i = 0; i < _memory_properties.memoryTypeCount; i++)
//			{
//				if ((typeBits & 1) == 1)
//				{
//					if ((_memory_properties.memoryTypes[i].propertyFlags &
// properties)
//== properties)
//					{
//						if (memTypeFound)
//						{
//							*memTypeFound = true;
//						}
//						return i;
//					}
//				}
//				typeBits >>= 1;
//			}
//
//			if (memTypeFound)
//			{
//				*memTypeFound = false;
//				return 0;
//			}
//			else
//			{
//				throw std::runtime_error("Could not find a matching memory
// type");
//			}
//		}
//
//		/**
//		* Get the index of a queue family that supports the requested queue
// flags
//		*
//		* @param queue_flags Queue flags to find a queue family index for
//		*
//		* @return Index of the queue family index that matches the flags
//		*
//		* @throw Throws an exception if no queue family index could be found
// that supports the requested flags
//		*/
//		uint32_t get_queue_family_index(VkQueueFlagBits queue_flags)
//		{
//			// Dedicated queue for compute
//			// Try to find a queue family index that supports compute but not
// graphics 			if (queueFlags & VK_QUEUE_COMPUTE_BIT)
//			{
//				for (uint32_t i = 0; i <
// static_cast<uint32_t>(_queue_family_properties.size()); i++)
//				{
//					if ((_queue_family_properties[i].queueFlags & queue_flags)
//&&
//((_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
//					{
//						return i;
//						break;
//					}
//				}
//			}
//
//			// Dedicated queue for transfer
//			// Try to find a queue family index that supports transfer but not
// graphics and compute 			if (queueFlags & VK_QUEUE_TRANSFER_BIT)
//			{
//				for (uint32_t i = 0; i <
// static_cast<uint32_t>(_queue_family_properties.size()); i++)
//				{
//					if ((_queue_family_properties[i].queueFlags & queue_flags)
//&&
//((_queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
//((_queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
//					{
//						return i;
//						break;
//					}
//				}
//			}
//
//			// For other queue types or if no separate compute queue is present,
// return the first one to support the requested flags 			for (uint32_t i =
// 0; i < static_cast<uint32_t>(_queue_family_properties.size()); i++)
//			{
//				if (_queue_family_properties[i].queueFlags & queue_flags)
//				{
//					return i;
//					break;
//				}
//			}
//
//			throw std::runtime_error("Could not find a matching queue family
// index");
//		}
//
//		/**
//		* Create the logical device based on the assigned physical device, also
// gets default queue family indices
//		*
//		* @param enabled_features Can be used to enable certain features upon
// device creation
//		* @param use_swap_chain Set to false for headless rendering to omit the
// swapchain device extensions
//		* @param requested_queue_types Bit flags specifying the queue types to
// be requested from the device
//		*
//		* @return VkResult of the device creation call
//		*/
//		VkResult create_logical_device(VkPhysicalDeviceFeatures
// enabled_features, std::vector<const char*> enabled_extensions, bool
// use_swap_chain = true, VkQueueFlags requested_queue_types =
// VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)
//		{
//			// Desired queues need to be requested upon logical device creation
//			// Due to differing queue family configurations of Vulkan
// implementations this can be a bit tricky, especially if the application
//			// requests different queue types
//
//			std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
//
//			// Get queue family indices for the requested queue family types
//			// Note that the indices may overlap depending on the implementation
//
//			const float default_queue_priority(0.0f);
//
//			// Graphics queue
//			if (requestedQueueTypes & VK_QUEUE_GRAPHICS_BIT)
//			{
//				_queue_family_indices._graphics =
// get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
// VkDeviceQueueCreateInfo queue_info{}; 				queueInfo.sType =
// VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
// queueInfo.queueFamilyIndex
// =
//_queue_family_indices._graphics; 				queueInfo.queueCount = 1;
//				queueInfo.pQueuePriorities = &defaultQueuePriority;
//				queueCreateInfos.push_back(queueInfo);
//			}
//			else
//			{
//				_queue_family_indices._graphics = VK_NULL_HANDLE;
//			}
//
//			// Dedicated compute queue
//			if (requestedQueueTypes & VK_QUEUE_COMPUTE_BIT)
//			{
//				_queue_family_indices._compute =
// get_queue_family_index(VK_QUEUE_COMPUTE_BIT); 				if
//(_queue_family_indices._compute != _queue_family_indices._graphics)
//				{
//					// If compute family index differs, we need an additional
// queue create info for the compute queue VkDeviceQueueCreateInfo queue_info{};
//					queueInfo.sType =
// VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
// queueInfo.queueFamilyIndex = _queue_family_indices._compute;
// queueInfo.queueCount =
// 1; 					queueInfo.pQueuePriorities = &defaultQueuePriority;
//					queueCreateInfos.push_back(queueInfo);
//				}
//			}
//			else
//			{
//				// Else we use the same queue
//				_queue_family_indices._compute =
//_queue_family_indices._graphics;
//			}
//
//			// Dedicated transfer queue
//			if (requestedQueueTypes & VK_QUEUE_TRANSFER_BIT)
//			{
//				_queue_family_indices._transfer =
// get_queue_family_index(VK_QUEUE_TRANSFER_BIT); 				if
//((_queue_family_indices._transfer != _queue_family_indices._graphics) &&
//(_queue_family_indices._transfer != _queue_family_indices._compute))
//				{
//					// If compute family index differs, we need an additional
// queue create info for the compute queue VkDeviceQueueCreateInfo queue_info{};
//					queueInfo.sType =
// VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
// queueInfo.queueFamilyIndex = _queue_family_indices._transfer;
// queueInfo.queueCount =
// 1; 					queueInfo.pQueuePriorities = &defaultQueuePriority;
//					queueCreateInfos.push_back(queueInfo);
//				}
//			}
//			else
//			{
//				// Else we use the same queue
//				_queue_family_indices._transfer =
//_queue_family_indices._graphics;
//			}
//
//			// Create the logical device representation
//			std::vector<const char*> device_extensions(enabledExtensions);
//			if (useSwapChain)
//			{
//				// If the device will be used for presenting to a display via a
// swapchain we need to request the swapchain extension
//				deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
//			}
//
//			VkDeviceCreateInfo device_create_info = {};
//			deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//			deviceCreateInfo.queueCreateInfoCount =
// static_cast<uint32_t>(queueCreateInfos.size());;
//			deviceCreateInfo.pQueueCreateInfos = queue_create_infos.data();
//			deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
//
//			if (deviceExtensions.size() > 0)
//			{
//				deviceCreateInfo.enabledExtensionCount =
//(uint32_t)deviceExtensions.size();
// deviceCreateInfo.ppEnabledExtensionNames = device_extensions.data();
//			}
//
//			VkResult result = vk_create_device(_physical_device,
//&deviceCreateInfo, nullptr, &_logical_device);
//
//			if (result == VK_SUCCESS)
//			{
//				// Create a default command pool for graphics command buffers
//				_command_pool =
// create_command_pool(_queue_family_indices._graphics);
//			}
//
//			_enabled_features = enabled_features;
//
//			return result;
//		}
//
//		/**
//		* Create a buffer on the device
//		*
//		* @param usage_flags Usage flag bitmask for the buffer (i.e. index,
// vertex, uniform buffer)
//		* @param memory_property_flags Memory properties for this buffer (i.e.
// device local, host visible, coherent)
//		* @param size Size of the buffer in byes
//		* @param buffer Pointer to the buffer handle acquired by the function
//		* @param memory Pointer to the memory handle acquired by the function
//		* @param data Pointer to the data that should be copied to the buffer
// after creation (optional, if not set, no data is copied over)
//		*
//		* @return VK_SUCCESS if buffer handle and memory have been created and
//(optionally passed) data has been copied
//		*/
//		VkResult create_buffer(VkBufferUsageFlags usage_flags,
// VkMemoryPropertyFlags memory_property_flags, VkDeviceSize size, VkBuffer
//*buffer, VkDeviceMemory *memory, void *data = nullptr)
//		{
//			// Create the buffer handle
//			VkBufferCreateInfo buffer_create_info =
// vks::initializers::bufferCreateInfo(usageFlags, size);
//			bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//			vkCreateBuffer(_logical_device, &bufferCreateInfo, nullptr, buffer);
//
//			// Create the memory backing up the buffer handle
//			VkMemoryRequirements mem_reqs;
//			VkMemoryAllocateInfo mem_alloc =
// vks::initializers::memoryAllocateInfo();
//			vkGetBufferMemoryRequirements(_logical_device, *buffer, &memReqs);
//			memAlloc.allocationSize = mem_reqs.size;
//			// Find a memory type index that fits the properties of the buffer
//			memAlloc.memoryTypeIndex = get_memory_type(memReqs.memoryTypeBits,
// memory_property_flags); 			vkAllocateMemory(_logical_device, &memAlloc,
// nullptr, memory);
//
//			// If a pointer to the buffer data has been passed, map the buffer
// and copy over the data 			if (data != nullptr)
//			{
//				void *mapped;
//				vkMapMemory(_logical_device, *memory, 0, size, 0, &mapped);
//				memcpy(mapped, data, (size_t)size);
//				// If host coherency hasn't been requested, do a manual flush to
// make writes visible 				if ((memoryPropertyFlags &
// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
//				{
//					VkMappedMemoryRange mapped_range =
// vks::initializers::mappedMemoryRange(); 					mappedRange.memory =
// *memory; 					mappedRange.offset = 0; 					mappedRange.size
// = size;
//					vkFlushMappedMemoryRanges(_logical_device, 1, &mappedRange);
//				}
//				vkUnmapMemory(_logical_device, *memory);
//			}
//
//			// Attach the memory to the buffer object
//			vkBindBufferMemory(_logical_device, *buffer, *memory, 0);
//
//			return VK_SUCCESS;
//		}
//
//		/**
//		* Create a buffer on the device
//		*
//		* @param usage_flags Usage flag bitmask for the buffer (i.e. index,
// vertex, uniform buffer)
//		* @param memory_property_flags Memory properties for this buffer (i.e.
// device local, host visible, coherent)
//		* @param buffer Pointer to a vk::Vulkan buffer object
//		* @param size Size of the buffer in byes
//		* @param data Pointer to the data that should be copied to the buffer
// after creation (optional, if not set, no data is copied over)
//		*
//		* @return VK_SUCCESS if buffer handle and memory have been created and
//(optionally passed) data has been copied
//		*/
//		VkResult create_buffer(VkBufferUsageFlags usage_flags,
// VkMemoryPropertyFlags memory_property_flags, vks::Buffer *buffer,
// VkDeviceSize size, void *data = nullptr)
//		{
//			buffer->_device = _logical_device;
//
//			// Create the buffer handle
//			VkBufferCreateInfo buffer_create_info =
// vks::initializers::bufferCreateInfo(usageFlags, size);
//			vkCreateBuffer(_logical_device, &bufferCreateInfo, nullptr,
//&buffer->_buffer);
//
//			// Create the memory backing up the buffer handle
//			VkMemoryRequirements mem_reqs;
//			VkMemoryAllocateInfo mem_alloc =
// vks::initializers::memoryAllocateInfo();
//			vkGetBufferMemoryRequirements(_logical_device, buffer->_buffer,
//&memReqs); 			memAlloc.allocationSize = mem_reqs.size;
//			// Find a memory type index that fits the properties of the buffer
//			memAlloc.memoryTypeIndex = get_memory_type(memReqs.memoryTypeBits,
// memory_property_flags); 			vkAllocateMemory(_logical_device, &memAlloc,
// nullptr, &buffer->_memory);
//
//			buffer->_alignment = mem_reqs.alignment;
//			buffer->_size = mem_alloc.allocationSize;
//			buffer->_usage_flags = usage_flags;
//			buffer->_memory_property_flags = memory_property_flags;
//
//			// If a pointer to the buffer data has been passed, map the buffer
// and copy over the data 			if (data != nullptr)
//			{
//				buffer->map();
//				memcpy(buffer->_mapped, data, (size_t)size);
//				if ((memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
//== 0) 					buffer->flush();
//
//				buffer->unmap();
//			}
//
//			// Initialize a default descriptor that covers the whole buffer size
//			buffer->setupDescriptor();
//
//			// Attach the memory to the buffer object
//			return buffer->bind();
//		}
//
//		/**
//		* Copy buffer data from src to dst using VkCmdCopyBuffer
//		*
//		* @param src Pointer to the source buffer to copy from
//		* @param dst Pointer to the destination buffer to copy tp
//		* @param queue Pointer
//		* @param copy_region (Optional) Pointer to a copy region, if NULL, the
// whole buffer is copied
//		*
//		* @note Source and destionation pointers must have the approriate
// transfer usage flags set (TRANSFER_SRC / TRANSFER_DST)
//		*/
//		void copy_buffer(vks::Buffer *src, vks::Buffer *dst, VkQueue queue,
// VkBufferCopy *copyRegion = nullptr)
//		{
//			assert(dst->_size <= src->_size);
//			assert(src->_buffer);
//			VkCommandBuffer copy_cmd =
// create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
// VkBufferCopy buffer_copy{}; 			if (copyRegion == nullptr)
//			{
//				bufferCopy.size = src->_size;
//			}
//			else
//			{
//				bufferCopy = *copyRegion;
//			}
//
//			vkCmdCopyBuffer(copyCmd, src->_buffer, dst->_buffer, 1,
//&bufferCopy);
//
//			flushCommandBuffer(copyCmd, queue);
//		}
//
//		/**
//		* Create a command pool for allocation command buffers from
//		*
//		* @param queue_family_index Family index of the queue to create the
// command pool for
//		* @param create_flags (Optional) Command pool creation flags (Defaults
// to VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
//		*
//		* @note Command buffers allocated from the created pool can only be
// submitted to a queue with the same family index
//		*
//		* @return A handle to the created command buffer
//		*/
//		VkCommandPool create_command_pool(uint32_t queue_family_index,
// VkCommandPoolCreateFlags create_flags =
// VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT)
//		{
//			VkCommandPoolCreateInfo cmd_pool_info = {};
//			cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//			cmdPoolInfo.queueFamilyIndex = queue_family_index;
//			cmdPoolInfo.flags = create_flags;
//			VkCommandPool cmd_pool;
//			vkCreateCommandPool(_logical_device, &cmdPoolInfo, nullptr,
//&cmdPool); 			return cmd_pool;
//		}
//
//		/**
//		* Allocate a command buffer from the command pool
//		*
//		* @param level Level of the new command buffer (primary or secondary)
//		* @param (Optional) begin If true, recording on the new command buffer
// will be started (vkBeginCommandBuffer) (Defaults to false)
//		*
//		* @return A handle to the allocated command buffer
//		*/
//		VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool
// begin = false)
//		{
//			VkCommandBufferAllocateInfo cmd_buf_allocate_info =
// vks::initializers::command_buffer_allocate_info(_command_pool, level, 1);
//
//			VkCommandBuffer cmd_buffer;
//			vkAllocateCommandBuffers(_logical_device, &cmdBufAllocateInfo,
//&cmdBuffer);
//
//			// If requested, also start recording for the new command buffer
//			if (begin)
//			{
//				VkCommandBufferBeginInfo cmd_buf_info =
// vks::initializers::commandBufferBeginInfo();
// vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo);
//			}
//
//			return cmd_buffer;
//		}
//
//		/**
//		* Finish command buffer recording and submit it to a queue
//		*
//		* @param command_buffer Command buffer to flush
//		* @param queue Queue to submit the command buffer to
//		* @param free (Optional) Free the command buffer once it has been
// submitted (Defaults to true)
//		*
//		* @note The queue that the command buffer is submitted to must be from
// the same family index as the pool it was allocated from
//		* @note Uses a fence to ensure command buffer has finished executing
//		*/
//		void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue,
// bool free = true)
//		{
//			if (commandBuffer == VK_NULL_HANDLE)
//			{
//				return;
//			}
//
//			vkEndCommandBuffer(commandBuffer);
//
//			VkSubmitInfo submit_info = vks::initializers::submitInfo();
//			submitInfo.commandBufferCount = 1;
//			submitInfo.pCommandBuffers = &commandBuffer;
//
//			// Create fence to ensure that the command buffer has finished
// executing 			VkFenceCreateInfo fence_info =
// vks::initializers::fenceCreateInfo(VK_FLAGS_NONE); 			VkFence fence;
//			vkCreateFence(_logical_device, &fenceInfo, nullptr, &fence);
//
//			// Submit to the queue
//			vkQueueSubmit(queue, 1, &submitInfo, fence);
//			// Wait for the fence to signal that command buffer has finished
// executing 			vkWaitForFences(_logical_device, 1, &fence, VK_TRUE,
// DEFAULT_FENCE_TIMEOUT);
//
//			vkDestroyFence(_logical_device, fence, nullptr);
//
//			if (free)
//			{
//				vkFreeCommandBuffers(_logical_device, _command_pool, 1,
//&commandBuffer);
//			}
//		}
//
//		/**
//		* Check if an extension is supported by the (physical device)
//		*
//		* @param extension Name of the extension to check
//		*
//		* @return True if the extension is supported (present in the list read
// at device creation time)
//		*/
//		bool extension_supported(std::string extension)
//		{
//			return (std::find(_supported_extensions.begin(),
//_supported_extensions.end(), extension) != _supported_extensions.end());
//		}
//
//	};
//}
//
// class VulkanExamplePortBase
//{
// private:
//	// Get window title with example name, device, et.
//	std::string get_window_title();
//	/** brief Indicates that the view (position, rotation) has changed and
// buffers containing camera matrices need to be updated */ 	bool
// _view_updated = false;
//	// Destination dimensions for resizing the window
//	uint32_t _dest_width;
//	uint32_t _dest_height;
//	bool _resizing = false;
// protected:
//	// Frame counter to display fps
//	uint32_t _frame_counter = 0;
//	uint32_t _last_f_p_s = 0;
//	std::chrono::time_point<std::chrono::high_resolution_clock> _last_timestamp;
//	// Vulkan instance, stores all per-application states
//	VkInstance _instance;
//	// Physical device (GPU) that Vulkan will ise
//	VkPhysicalDevice _physical_device;
//	// Stores physical device properties (for e.g. checking device limits)
//	VkPhysicalDeviceProperties _device_properties;
//	// Stores the features available on the selected physical device (for e.g.
// checking if a feature is available) 	VkPhysicalDeviceFeatures
// _device_features;
//	// Stores all available memory (type) properties for the physical device
//	VkPhysicalDeviceMemoryProperties _device_memory_properties;
//	/**
//	* Set of physical device features to be enabled for this example (must be
// set in the derived constructor)
//	*
//	* @note By default no phyiscal device features are enabled
//	*/
//	VkPhysicalDeviceFeatures _enabled_features{};
//	/** @brief Set of device extensions to be enabled for this example (must be
// set in the derived constructor) */ 	std::vector<const char*>
//_enabled_device_extensions; 	std::vector<const char*>
//_enabled_instance_extensions;
//	/** @brief Logical device, application's view of the physical device (GPU)
//*/
//	// todo: getter? should always point to VulkanDevice->device
//	VkDevice _device;
//	// Handle to the device graphics queue that command buffers are submitted to
//	VkQueue _queue;
//	// Depth buffer format (selected during Vulkan initialization)
//	VkFormat _depth_format;
//	// Command buffer pool
//	VkCommandPool _cmd_pool;
//	/** @brief Pipeline stages used to wait at for graphics queue submissions */
//	VkPipelineStageFlags _submit_pipeline_stages =
// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//	// Contains command buffers and semaphores to be presented to the queue
//	VkSubmitInfo _submit_info;
//	// Command buffers used for rendering
//	std::vector<VkCommandBuffer> _draw_cmd_buffers;
//	// Global render pass for frame buffer writes
//	VkRenderPass _render_pass;
//	// List of available frame buffers (same as number of swap chain images)
//	std::vector<VkFramebuffer> _frame_buffers;
//	// Active frame buffer index
//	uint32_t _current_buffer = 0;
//	// Descriptor set pool
//	VkDescriptorPool _descriptor_pool = VK_NULL_HANDLE;
//	// List of shader modules created (stored for cleanup)
//	std::vector<VkShaderModule> _shader_modules;
//	// Pipeline cache object
//	VkPipelineCache _pipeline_cache;
//	// Wraps the swap chain to present images (framebuffers) to the windowing
// system 	VulkanSwapChain _swap_chain;
//	// Synchronization semaphores
//	struct {
//		// Swap chain image presentation
//		VkSemaphore _present_complete;
//		// Command buffer submission and execution
//		VkSemaphore _render_complete;
//	} _semaphores;
//	std::vector<VkFence> _wait_fences;
// public:
//	bool _prepared = false;
//	uint32_t _width = 1280;
//	uint32_t _height = 720;
//
//
//	/** @brief Last frame time measured using a high performance timer (if
// available) */ 	float _frame_timer = 1.0f;
//	/** @brief Returns os specific base asset path (for shaders, models,
// textures) */ 	const std::string get_asset_path();
//
//	vks::Benchmark _benchmark;
//
//	/** @brief Encapsulated physical and logical vulkan device */
//	vks::VulkanDevice *_vulkan_device;
//
//	/** @brief Example settings that can be changed e.g. by command line
// arguments */ 	struct Settings {
//		/** @brief Activates validation layers (and message output) when set to
// true */ 		bool _validation = false;
//		/** @brief Set to true if fullscreen mode has been requested via command
// line */ 		bool _fullscreen = false;
//		/** @brief Set to true if v-sync will be forced for the swapchain */
//		bool _vsync = false;
//	} _settings;
//
//	VkClearColorValue _default_clear_color = { { 0.025f, 0.025f, 0.025f, 1.0f }
//};
//
//	// Defines a frame rate independent timer value clamped from -1.0...1.0
//	// For use in animations, rotations, etc.
//	float _timer = 0.0f;
//	// Multiplier for speeding up (or slowing down) the global timer
//	float _timer_speed = 0.25f;
//
//	bool _paused = false;
//
//	Camera _camera;
//
//	glm::vec3 _rotation = glm::vec3();
//	glm::vec3 _camera_pos = glm::vec3();
//
//	std::string _title = "Vulkan Example";
//	std::string _name = "vulkanExample";
//	uint32_t _api_version = VK_API_VERSION_1_0;
//
//	struct
//	{
//		VkImage _image;
//		VkDeviceMemory _mem;
//		VkImageView _view;
//	} _depth_stencil;
//
//	struct {
//		glm::vec2 _axis_left = glm::vec2(0.0f);
//		glm::vec2 _axis_right = glm::vec2(0.0f);
//	} _game_pad_state;
//
//	HWND _window;
//	HINSTANCE _window_instance;
//
//	// Default ctor
//	VulkanExamplePortBase(bool enable_validation);
//
//	// dtor
//	virtual ~VulkanExamplePortBase();
//
//	// Setup the vulkan instance, enable required extensions and connect to the
// physical device (GPU) 	bool init_vulkan();
//
//	HWND setup_window(HINSTANCE hinstance, WNDPROC wndproc);
//	void handle_messages(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM
// l_param);
//
//	/**
//	* Create the application wide Vulkan instance
//	*
//	* @note Virtual, can be overriden by derived example class for custom
// instance creation
//	*/
//	virtual VkResult create_instance(bool enable_validation);
//
//	// Pure virtual render function (override in derived class)
//	virtual void render() = 0;
//
//	void create_synchronization_primitives();
//
//	// Creates a new (graphics) command pool object storing command buffers
//	void create_command_pool();
//	// Setup default depth and stencil views
//	virtual void setup_depth_stencil();
//	// Create framebuffers for all requested swap chain images
//	// Can be overriden in derived class to setup a custom framebuffer (e.g. for
// MSAA) 	virtual void setup_frame_buffer();
//	// Setup a default render pass
//	// Can be overriden in derived class to setup a custom render pass (e.g. for
// MSAA) 	virtual void setup_render_pass();
//
//	/** @brief (Virtual) Called after the physical device features have been
// read, can be used to set features to enable on the device */ 	virtual void
// get_enabled_features();
//
//	// Connect and prepare the swap chain
//	void init_swapchain();
//	// Create swap chain images
//	void setup_swap_chain();
//
//	// Create command buffers for drawing commands
//	void create_command_buffers();
//	// Destroy all command buffers and set their handles to VK_NULL_HANDLE
//	// May be necessary during runtime if options are toggled
//	void destroy_command_buffers();
//
//	// Command buffer creation
//	// Creates and returns a new command buffer
//	VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool
// begin);
//	// End the command buffer, submit it to the queue and free (if requested)
//	// Note : Waits for the queue to become idle
//	void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue,
// bool free);
//
//	// Create a cache pool for rendering pipelines
//	void create_pipeline_cache();
//
//	// Prepare commonly used Vulkan functions
//	virtual void prepare();
//
//	// Load a SPIR-V shader
//	VkPipelineShaderStageCreateInfo load_shader(std::string file_name,
// VkShaderStageFlagBits stage);
//
//	// Start the main render loop
//	void render_loop();
//
//	// Render one frame of a render loop on platforms that sync rendering
//	void render_frame();
//
//	// Prepare the frame for workload submission
//	// - Acquires the next image from the swap chain
//	// - Sets the default wait and signal semaphores
//	void prepare_frame();
//
//	// Submit the frames' workload
//	void submit_frame();
//
//};
//
//
//#define VERTEX_BUFFER_BIND_ID 0
//#define ENABLE_VALIDATION false
//#define PARTICLE_COUNT 256 * 1024
//
//
// namespace vks
//{
//	/** @brief Vulkan texture base class */
//	class Texture {
//	public:
//		vks::VulkanDevice *_device;
//		VkImage _image;
//		VkImageLayout _image_layout;
//		VkDeviceMemory _device_memory;
//		VkImageView _view;
//		uint32_t _width, _height;
//		uint32_t _mip_levels;
//		uint32_t _layer_count;
//		VkDescriptorImageInfo _descriptor;
//
//		/** @brief Optional sampler to use with this texture */
//		VkSampler _sampler;
//
//		/** @brief Update image descriptor from current sampler, view and image
// layout */ 		void update_descriptor()
//		{
//			_descriptor.sampler = _sampler;
//			_descriptor.imageView = _view;
//			_descriptor.imageLayout = _image_layout;
//		}
//
//		/** @brief Release all Vulkan resources held by this texture */
//		void destroy()
//		{
//			vkDestroyImageView(_device->_logical_device, _view, nullptr);
//			vkDestroyImage(_device->_logical_device, _image, nullptr);
//			if (_sampler)
//			{
//				vkDestroySampler(_device->_logical_device, _sampler, nullptr);
//			}
//			vkFreeMemory(_device->_logical_device, _device_memory, nullptr);
//		}
//	};
//
//	/** @brief 2D texture */
//	class Texture2D : public Texture {
//	public:
//		/**
//		* Load a 2D texture including all mip levels
//		*
//		* @param file_name File to load (supports .ktx and .dds)
//		* @param format Vulkan format of the image data stored in the file
//		* @param device Vulkan device to create the texture on
//		* @param copy_queue Queue used for the texture staging copy commands
//(must support transfer)
//		* @param (Optional) image_usage_flags Usage flags for the texture's
// image (defaults to VK_IMAGE_USAGE_SAMPLED_BIT)
//		* @param (Optional) image_layout Usage layout for the texture (defaults
// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
//		* @param (Optional) force_linear Force linear tiling (not advised,
// defaults to false)
//		*
//		*/
//		void load_from_file(
//			std::string file_name,
//			VkFormat format,
//			vks::VulkanDevice *device,
//			VkQueue copy_queue,
//			VkImageUsageFlags image_usage_flags = VK_IMAGE_USAGE_SAMPLED_BIT,
//			VkImageLayout image_layout =
// VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 			bool force_linear =
// false)
//		{
//			if (!tools::fileExists(file_name)) {
//				tools::exitFatal("Could not load texture from " + file_name +
//"\n\nThe file may be part of the additional asset pack.\n\nRun
// \"download_assets.py\" in the repository root to download the latest
// version.", -1);
//			}
//			gli::texture2d tex_2_d(gli::load(file_name.c_str()));
//			assert(!tex2D.empty());
//
//			_device = device;
//			_width = static_cast<uint32_t>(tex2D[0].extent().x);
//			_height = static_cast<uint32_t>(tex2D[0].extent().y);
//			_mip_levels = static_cast<uint32_t>(tex2D.levels());
//
//			// Get device properites for the requested texture format
//			VkFormatProperties format_properties;
//			vkGetPhysicalDeviceFormatProperties(device->_physical_device,
// format, &formatProperties);
//
//			// Only use linear tiling if requested (and supported by the device)
//			// Support for linear tiling is mostly limited, so prefer to use
//			// optimal tiling instead
//			// On most implementations linear tiling will only support a very
//			// limited amount of formats and features (mip maps, cubemaps,
// arrays, etc.) 			VkBool32 use_staging = !forceLinear;
//
//			VkMemoryAllocateInfo mem_alloc_info =
// vks::initializers::memoryAllocateInfo(); 			VkMemoryRequirements
// mem_reqs;
//
//			// Use a separate command buffer for texture loading
//			VkCommandBuffer copy_cmd =
// device->createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
//
//			if (useStaging)
//			{
//				// Create a host-visible staging buffer that contains the raw
// image data 				VkBuffer staging_buffer; VkDeviceMemory
// staging_memory;
//
//				VkBufferCreateInfo buffer_create_info =
// vks::initializers::bufferCreateInfo(); 				bufferCreateInfo.size =
// tex_2_d.size();
//				// This buffer is used as a transfer source for the buffer copy
//				bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
//				bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//				vkCreateBuffer(device->_logical_device, &bufferCreateInfo,
// nullptr, &stagingBuffer);
//
//				// Get memory requirements for the staging buffer (alignment,
// memory type bits)
// vkGetBufferMemoryRequirements(device->_logical_device, staging_buffer,
// &memReqs);
//
//				memAllocInfo.allocationSize = mem_reqs.size;
//				// Get memory type index for a host visible buffer
//				memAllocInfo.memoryTypeIndex =
// device->getMemoryType(memReqs.memoryTypeBits,
// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//				vkAllocateMemory(device->_logical_device, &memAllocInfo,
// nullptr,
//&stagingMemory); 				vkBindBufferMemory(device->_logical_device,
// staging_buffer, staging_memory, 0);
//
//				// Copy texture data into staging buffer
//				uint8_t *data;
//				vkMapMemory(device->_logical_device, staging_memory, 0,
// mem_reqs.size, 0, (void **)&data); 				memcpy(data, tex_2_d.data(),
// tex_2_d.size()); 				vkUnmapMemory(device->_logical_device,
// staging_memory);
//
//				// Setup buffer copy regions for each mip level
//				std::vector<VkBufferImageCopy> buffer_copy_regions;
//				uint32_t offset = 0;
//
//				for (uint32_t i = 0; i < _mip_levels; i++)
//				{
//					VkBufferImageCopy buffer_copy_region = {};
//					bufferCopyRegion.imageSubresource.aspectMask =
// VK_IMAGE_ASPECT_COLOR_BIT;
// bufferCopyRegion.imageSubresource.mipLevel = i;
// bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
//					bufferCopyRegion.imageSubresource.layerCount = 1;
//					bufferCopyRegion.imageExtent.width =
// static_cast<uint32_t>(tex2D[i].extent().x);
//					bufferCopyRegion.imageExtent.height =
// static_cast<uint32_t>(tex2D[i].extent().y);
// bufferCopyRegion.imageExtent.depth = 1;
// bufferCopyRegion.bufferOffset = offset;
//
//					bufferCopyRegions.push_back(bufferCopyRegion);
//
//					offset += static_cast<uint32_t>(tex2D[i].size());
//				}
//
//				// Create optimal tiled target image
//				VkImageCreateInfo image_create_info =
// vks::initializers::image_create_info(); image_create_info.imageType =
// VK_IMAGE_TYPE_2D; 				image_create_info.format = format;
//				image_create_info.mipLevels = _mip_levels;
//				image_create_info.arrayLayers = 1;
//				image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
//				image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
//				image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//				image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//				image_create_info.extent = { _width, _height, 1 };
//				image_create_info.usage = image_usage_flags;
//				// Ensure that the TRANSFER_DST bit is set for staging
//				if (!(image_create_info.usage &
// VK_IMAGE_USAGE_TRANSFER_DST_BIT))
//				{
//					image_create_info.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
//				}
//				vkCreateImage(device->_logical_device, &image_create_info,
// nullptr,
//&_image);
//
//				vkGetImageMemoryRequirements(device->_logical_device, _image,
//&memReqs);
//
//				memAllocInfo.allocationSize = mem_reqs.size;
//
//				memAllocInfo.memoryTypeIndex =
// device->getMemoryType(memReqs.memoryTypeBits,
// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//				vkAllocateMemory(device->_logical_device, &memAllocInfo,
// nullptr,
//&_device_memory); 				vkBindImageMemory(device->_logical_device,
//_image, _device_memory, 0);
//
//				VkImageSubresourceRange subresource_range = {};
//				subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//				subresource_range.baseMipLevel = 0;
//				subresource_range.levelCount = _mip_levels;
//				subresource_range.layerCount = 1;
//
//				// Image barrier for optimal image (target)
//				// Optimal image will be used as destination for the copy
//				tools::set_image_layout(
//					copyCmd,
//					_image,
//					VK_IMAGE_LAYOUT_UNDEFINED,
//					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//					subresource_range);
//
//				// Copy mip levels from staging buffer
//				vkCmdCopyBufferToImage(
//					copyCmd,
//					stagingBuffer,
//					_image,
//					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//					static_cast<uint32_t>(bufferCopyRegions.size()),
//					bufferCopyRegions.data()
//				);
//
//				// Change texture image layout to shader read after all mip
// levels have been copied 				_image_layout = image_layout;
// tools::set_image_layout( 					copyCmd, _image,
//					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//					imageLayout,
//					subresource_range);
//
//				device->flushCommandBuffer(copyCmd, copy_queue);
//
//				// Clean up staging resources
//				vkFreeMemory(device->_logical_device, staging_memory, nullptr);
//				vkDestroyBuffer(device->_logical_device, staging_buffer,
// nullptr);
//			}
//			else
//			{
//				// Prefer using optimal tiling, as linear tiling
//				// may support only a small set of features
//				// depending on implementation (e.g. no mip maps, only one
// layer, etc.)
//
//				// Check if this support is supported for linear tiling
//				assert(formatProperties.linearTilingFeatures &
// VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT);
//
//				VkImage mappable_image;
//				VkDeviceMemory mappable_memory;
//
//				VkImageCreateInfo image_create_info =
// vks::initializers::image_create_info(); image_create_info.imageType =
// VK_IMAGE_TYPE_2D; 				image_create_info.format = format;
// image_create_info.extent = { _width, _height, 1 };
// image_create_info.mipLevels = 1; 				image_create_info.arrayLayers
// = 1; 				image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
// image_create_info.tiling
//= VK_IMAGE_TILING_LINEAR; 				image_create_info.usage =
//image_usage_flags; 				image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//				image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//
//				// Load mip map level 0 to linear tiling image
//				vkCreateImage(device->_logical_device, &image_create_info,
// nullptr, &mappableImage);
//
//				// Get memory requirements for this image
//				// like size and alignment
//				vkGetImageMemoryRequirements(device->_logical_device,
// mappable_image, &memReqs);
//				// Set memory allocation size to required memory size
//				memAllocInfo.allocationSize = mem_reqs.size;
//
//				// Get memory type that can be mapped to host memory
//				memAllocInfo.memoryTypeIndex =
// device->getMemoryType(memReqs.memoryTypeBits,
// VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
//
//				// Allocate host memory
//				vkAllocateMemory(device->_logical_device, &memAllocInfo,
// nullptr, &mappableMemory);
//
//				// Bind allocated image for use
//				vkBindImageMemory(device->_logical_device, mappable_image,
// mappable_memory, 0);
//
//				// Get sub resource layout
//				// Mip map count, array layer, etc.
//				VkImageSubresource sub_res = {};
//				subRes.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//				subRes.mipLevel = 0;
//
//				VkSubresourceLayout sub_res_layout;
//				void *data;
//
//				// Get sub resources layout
//				// Includes row pitch, size offsets, etc.
//				vkGetImageSubresourceLayout(device->_logical_device,
// mappable_image, &subRes, &subResLayout);
//
//				// Map image memory
//				vkMapMemory(device->_logical_device, mappable_memory, 0,
// mem_reqs.size, 0, &data);
//
//				// Copy image data into memory
//				memcpy(data, tex_2_d[subRes.mipLevel].data(),
// tex_2_d[subRes.mipLevel].size());
//
//				vkUnmapMemory(device->_logical_device, mappable_memory);
//
//				// Linear tiled images don't need to be staged
//				// and can be directly used as textures
//				_image = mappable_image;
//				_device_memory = mappable_memory;
//				_image_layout = image_layout;
//
//				// Setup image memory barrier
//				tools::set_image_layout(copyCmd, _image,
// VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, image_layout);
//
//				device->flushCommandBuffer(copyCmd, copy_queue);
//			}
//
//			// Create a defaultsampler
//			VkSamplerCreateInfo sampler_create_info = {};
//			samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//			samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
//			samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
//			samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//			samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//			samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//			samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//			samplerCreateInfo.mipLodBias = 0.0f;
//			samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
//			samplerCreateInfo.minLod = 0.0f;
//			// Max level-of-detail should match mip level count
//			samplerCreateInfo.maxLod = (useStaging) ? (float)_mip_levels : 0.0f;
//			// Only enable anisotropic filtering if enabled on the devicec
//			samplerCreateInfo.maxAnisotropy =
// device->_enabled_features.samplerAnisotropy ?
// device->_properties.limits.maxSamplerAnisotropy : 1.0f;
//			samplerCreateInfo.anisotropyEnable =
// device->_enabled_features.samplerAnisotropy;
// samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
// vkCreateSampler(device->_logical_device, &samplerCreateInfo, nullptr,
//&_sampler);
//
//			// Create image view
//			// Textures are not directly accessed by the shaders and
//			// are abstracted by image views containing additional
//			// information and sub resource ranges
//			VkImageViewCreateInfo view_create_info = {};
//			viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//			viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//			viewCreateInfo.format = format;
//			viewCreateInfo.components = { VK_COMPONENT_SWIZZLE_R,
// VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
//			viewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1,
// 0, 1 };
//			// Linear tiling usually won't support mip maps
//			// Only set mip map count if optimal tiling is used
//			viewCreateInfo.subresourceRange.levelCount = (useStaging) ?
//_mip_levels : 1; 			viewCreateInfo.image = _image;
// vkCreateImageView(device->_logical_device, &viewCreateInfo, nullptr, &_view);
//
//			// Update descriptor image info member that can be used for setting
// up descriptor sets 			updateDescriptor();
//		}
//	};
//}
//
// class VulkanExample : public VulkanExamplePortBase
//{
// public:
//	float _timer = 0.0f;
//	float _anim_start = 20.0f;
//
//	struct {
//		vks::Texture2D _particle;
//		vks::Texture2D _gradient;
//	} textures;
//
//	struct {
//		VkPipelineVertexInputStateCreateInfo _input_state;
//		std::vector<VkVertexInputBindingDescription> _binding_descriptions;
//		std::vector<VkVertexInputAttributeDescription> _attribute_descriptions;
//	} vertices;
//
//	// Resources for the graphics part of the example
//	struct {
//		VkDescriptorSetLayout _descriptor_set_layout;	// Particle system
// rendering shader binding layout
//		VkDescriptorSet _descriptor_set;				// Particle system
// rendering shader bindings 		VkPipelineLayout _pipeline_layout;
// // Layout of the
// graphics pipeline 		VkPipeline _pipeline;						//
// Particle rendering pipeline 	} _graphics;
//
//	// Resources for the compute part of the example
//	struct {
//		vks::Buffer _storage_buffer;					// (Shader) storage
// buffer object containing the particles 		vks::Buffer _uniform_buffer;
//// Uniform buffer object containing particle system parameters 		VkQueue
// _queue;
//// Separate queue for compute commands (queue family may differ from the one
// used for graphics) 		VkCommandPool
//_command_pool;					// Use a separate command pool (queue family
// may differ from the one used for graphics) 		VkCommandBuffer
// _command_buffer;
//// Command buffer storing the dispatch commands and barriers 		VkFence
///_fence; / Synchronization fence to avoid rewriting compute CB if still in use
//		VkDescriptorSetLayout _descriptor_set_layout;	// Compute shader
// binding
// layout 		VkDescriptorSet _descriptor_set;				// Compute
// shader bindings 		VkPipelineLayout _pipeline_layout;			// Layout of
// the compute pipeline
//		VkPipeline _pipeline;						// Compute pipeline for
// updating particle
// positions 		struct compute_u_b_o {							// Compute
// shader uniform block object
//			float _delta_t;							//		Frame delta time
//			float _dest_x;							//		x position of the
// attractor 			float _dest_y;							//		y
// position of the
// attractor 			int32_t _particle_count = PARTICLE_COUNT; 		} _ubo;
// } _compute;
//
//	// SSBO particle declaration
//	struct Particle {
//		glm::vec2 _pos;								// Particle position
//		glm::vec2 _vel;								// Particle velocity
//		glm::vec4 _gradient_pos;						// Texture coordiantes
// for the gradient ramp map
//	};
//
//	VulkanExample() : VulkanExamplePortBase(ENABLE_VALIDATION)
//	{
//		_title = "Compute shader particle system";
//	}
//
//	~VulkanExample()
//	{
//		// Graphics
//		vkDestroyPipeline(_device, _graphics._pipeline, nullptr);
//		vkDestroyPipelineLayout(_device, _graphics._pipeline_layout, nullptr);
//		vkDestroyDescriptorSetLayout(_device, _graphics._descriptor_set_layout,
// nullptr);
//
//		// Compute
//		_compute._storage_buffer.destroy();
//		_compute._uniform_buffer.destroy();
//		vkDestroyPipelineLayout(_device, _compute._pipeline_layout, nullptr);
//		vkDestroyDescriptorSetLayout(_device, _compute._descriptor_set_layout,
// nullptr); 		vkDestroyPipeline(_device, _compute._pipeline, nullptr);
//		vkDestroyFence(_device, _compute._fence, nullptr);
//		vkDestroyCommandPool(_device, _compute._command_pool, nullptr);
//
//		textures._particle.destroy();
//		textures._gradient.destroy();
//	}
//
//	void load_assets()
//	{
//		textures._particle.loadFromFile(getAssetPath() +
//"textures/particle01_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM, _vulkan_device,
//_queue); 		textures._gradient.loadFromFile(getAssetPath() +
//"textures/particle_gradient_rgba.ktx", VK_FORMAT_R8G8B8A8_UNORM,
//_vulkan_device, _queue);
//	}
//
//	void build_command_buffers()
//	{
//		// Destroy command buffers if already present
//		for (auto& cmd_buffer : _draw_cmd_buffers)
//		{
//			if (cmdBuffer == VK_NULL_HANDLE)
//			{
//				destroyCommandBuffers();
//				createCommandBuffers();
//			}
//		}
//
//		VkCommandBufferBeginInfo cmd_buf_info =
// vks::initializers::commandBufferBeginInfo();
//
//		VkClearValue clear_values[2];
//		clearValues[0].color = _default_clear_color;
//		clearValues[1].depthStencil = { 1.0f, 0 };
//
//		VkRenderPassBeginInfo render_pass_begin_info =
// vks::initializers::renderPassBeginInfo(); renderPassBeginInfo.renderPass =
// _render_pass; 		renderPassBeginInfo.renderArea.offset.x = 0;
//		renderPassBeginInfo.renderArea.offset.y = 0;
//		renderPassBeginInfo.renderArea.extent.width = _width;
//		renderPassBeginInfo.renderArea.extent.height = _height;
//		renderPassBeginInfo.clearValueCount = 2;
//		renderPassBeginInfo.pClearValues = clear_values;
//
//		int32_t cmd_size = (int32_t)_draw_cmd_buffers.size();
//
//		for (int32_t i = 0; i < cmd_size; ++i)
//		{
//			// Set target frame buffer
//			renderPassBeginInfo.framebuffer = _frame_buffers[i];
//
//			vkBeginCommandBuffer(_draw_cmd_buffers[i], &cmdBufInfo);
//
//			// Draw the particle system using the update vertex buffer
//
//			vkCmdBeginRenderPass(_draw_cmd_buffers[i], &renderPassBeginInfo,
// VK_SUBPASS_CONTENTS_INLINE);
//
//			VkViewport viewport = vks::initializers::viewport((float)_width,
//(float)_height, 0.0f, 1.0f); 			vkCmdSetViewport(_draw_cmd_buffers[i],
// 0, 1, &viewport);
//
//			VkRect2D scissor = vks::initializers::rect2D(_width, _height, 0, 0);
//			vkCmdSetScissor(_draw_cmd_buffers[i], 0, 1, &scissor);
//
//			vkCmdBindPipeline(_draw_cmd_buffers[i],
// VK_PIPELINE_BIND_POINT_GRAPHICS, _graphics._pipeline);
// vkCmdBindDescriptorSets(_draw_cmd_buffers[i],
// VK_PIPELINE_BIND_POINT_GRAPHICS,
// _graphics._pipeline_layout, 0, 1,
//&_graphics._descriptor_set, 0, NULL);
//
//			VkDeviceSize offsets[1] = { 0 };
//			vkCmdBindVertexBuffers(_draw_cmd_buffers[i], VERTEX_BUFFER_BIND_ID,
// 1,
//&_compute._storage_buffer._buffer, offsets);
// vkCmdDraw(_draw_cmd_buffers[i], PARTICLE_COUNT, 1, 0, 0);
//
//			vkCmdEndRenderPass(_draw_cmd_buffers[i]);
//
//			vkEndCommandBuffer(_draw_cmd_buffers[i]);
//		}
//
//	}
//
//	void build_compute_command_buffer()
//	{
//		VkCommandBufferBeginInfo cmd_buf_info =
// vks::initializers::commandBufferBeginInfo();
//
//		vkBeginCommandBuffer(_compute._command_buffer, &cmdBufInfo);
//
//		// Compute particle movement
//
//		// Add memory barrier to ensure that the (graphics) vertex shader has
// fetched attributes before compute starts to write to the buffer
//		VkBufferMemoryBarrier buffer_barrier =
// vks::initializers::buffer_memory_barrier(); 		bufferBarrier.buffer =
//_compute._storage_buffer._buffer; 		bufferBarrier.size =
//_compute._storage_buffer._descriptor.range; 		bufferBarrier.srcAccessMask
//= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;						// Vertex shader
// invocations have finished reading from the buffer
// bufferBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
// // Compute shader wants to write to the buffer
//		// Compute and graphics queue may have different queue families (see
// VulkanDevice::createLogicalDevice)
//		// For the barrier to work across different queues, we need to set their
// family indices 		bufferBarrier.srcQueueFamilyIndex =
//_vulkan_device->_queue_family_indices._graphics;			// Required as
// compute and graphics queue may have different families
//		bufferBarrier.dstQueueFamilyIndex =
//_vulkan_device->_queue_family_indices._compute;			// Required as
// compute and graphics queue may have different families
//
//		vkCmdPipelineBarrier(
//			_compute._command_buffer,
//			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
//			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
//			VK_FLAGS_NONE,
//			0, nullptr,
//			1, &bufferBarrier,
//			0, nullptr);
//
//		vkCmdBindPipeline(_compute._command_buffer,
// VK_PIPELINE_BIND_POINT_COMPUTE, _compute._pipeline);
//		vkCmdBindDescriptorSets(_compute._command_buffer,
// VK_PIPELINE_BIND_POINT_COMPUTE, _compute._pipeline_layout, 0, 1,
//&_compute._descriptor_set, 0, 0);
//
//		// Dispatch the compute job
//		vkCmdDispatch(_compute._command_buffer, PARTICLE_COUNT / 256, 1, 1);
//
//		// Add memory barrier to ensure that compute shader has finished writing
// to the buffer
//		// Without this the (rendering) vertex shader may display incomplete
// results (partial data from last frame) 		bufferBarrier.srcAccessMask =
// VK_ACCESS_SHADER_WRITE_BIT;								// Compute shader
// has finished writes to the buffer 		bufferBarrier.dstAccessMask =
// VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;						// Vertex shader
// invocations want to read from the buffer 		bufferBarrier.buffer =
//_compute._storage_buffer._buffer; 		bufferBarrier.size =
//_compute._storage_buffer._descriptor.range;
//		// Compute and graphics queue may have different queue families (see
// VulkanDevice::createLogicalDevice)
//		// For the barrier to work across different queues, we need to set their
// family indices 		bufferBarrier.srcQueueFamilyIndex =
//_vulkan_device->_queue_family_indices._compute;			// Required as
// compute and graphics queue may have different families
//		bufferBarrier.dstQueueFamilyIndex =
//_vulkan_device->_queue_family_indices._graphics;			// Required as
// compute and graphics queue may have different families
//
//		vkCmdPipelineBarrier(
//			_compute._command_buffer,
//			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
//			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
//			VK_FLAGS_NONE,
//			0, nullptr,
//			1, &bufferBarrier,
//			0, nullptr);
//
//		vkEndCommandBuffer(_compute._command_buffer);
//	}
//
//	// Setup and fill the compute shader storage buffers containing the
// particles 	void prepare_storage_buffers()
//	{
//		std::default_random_engine rnd_engine(_benchmark._active ? 0 :
//(unsigned)time(nullptr)); 		std::uniform_real_distribution<float>
// rnd_dist(-1.0f, 1.0f);
//
//		// Initial particle positions
//		std::vector<Particle> particle_buffer(PARTICLE_COUNT);
//		for (auto& particle : particle_buffer) {
//			particle._pos = glm::vec2(rndDist(rndEngine), rnd_dist(rndEngine));
//			particle._vel = glm::vec2(0.0f);
//			particle._gradient_pos.x = particle._pos.x / 2.0f;
//		}
//
//		VkDeviceSize storage_buffer_size = particle_buffer.size() *
// sizeof(Particle);
//
//		// Staging
//		// SSBO won't be changed on the host after upload so copy to device
// local memory
//
//		vks::Buffer staging_buffer;
//
//		_vulkan_device->createBuffer(
//			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 			&stagingBuffer,
// storageBufferSize, 			particleBuffer.data());
//
//		_vulkan_device->createBuffer(
//			// The SSBO will be used as a storage buffer for the compute
// pipeline and as a vertex buffer in the graphics pipeline
//			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
// VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
//			&_compute._storage_buffer,
//			storageBufferSize);
//
//		// Copy to staging buffer
//		VkCommandBuffer copy_cmd =
// VulkanExamplePortBase::createCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY,
// true); 		VkBufferCopy copy_region = {}; 		copyRegion.size =
// storage_buffer_size; 		vkCmdCopyBuffer(copyCmd, staging_buffer._buffer,
//_compute._storage_buffer._buffer, 1, &copyRegion);
//		VulkanExamplePortBase::flushCommandBuffer(copyCmd, _queue, true);
//
//		stagingBuffer.destroy();
//
//		// Binding description
//		vertices._binding_descriptions.resize(1);
//		vertices._binding_descriptions[0] =
//			vks::initializers::vertexInputBindingDescription(
//				VERTEX_BUFFER_BIND_ID,
//				sizeof(Particle),
//				VK_VERTEX_INPUT_RATE_VERTEX);
//
//		// Attribute descriptions
//		// Describes memory layout and shader positions
//		vertices._attribute_descriptions.resize(2);
//		// Location 0 : Position
//		vertices._attribute_descriptions[0] =
//			vks::initializers::vertexInputAttributeDescription(
//				VERTEX_BUFFER_BIND_ID,
//				0,
//				VK_FORMAT_R32G32_SFLOAT,
//				offsetof(Particle, _pos));
//		// Location 1 : Gradient position
//		vertices._attribute_descriptions[1] =
//			vks::initializers::vertexInputAttributeDescription(
//				VERTEX_BUFFER_BIND_ID,
//				1,
//				VK_FORMAT_R32G32B32A32_SFLOAT,
//				offsetof(Particle, _gradient_pos));
//
//		// Assign to vertex buffer
//		vertices._input_state =
// vks::initializers::pipeline_vertex_input_state_create_info();
//		vertices._input_state.vertexBindingDescriptionCount =
// static_cast<uint32_t>(vertices._binding_descriptions.size());
//		vertices._input_state.pVertexBindingDescriptions =
// vertices._binding_descriptions.data();
//		vertices._input_state.vertexAttributeDescriptionCount =
// static_cast<uint32_t>(vertices._attribute_descriptions.size());
//		vertices._input_state.pVertexAttributeDescriptions =
// vertices._attribute_descriptions.data();
//	}
//
//	void setup_descriptor_pool()
//	{
//		std::vector<Vkdescriptor_pool_size> pool_sizes =
//		{
//			vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
// 1),
// vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1),
//			vks::initializers::descriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
// 2)
//		};
//
//		VkDescriptorPoolCreateInfo descriptor_pool_info =
//			vks::initializers::descriptorPoolCreateInfo(
//				static_cast<uint32_t>(poolSizes.size()),
//				poolSizes.data(),
//				2);
//
//		vkCreateDescriptorPool(_device, &descriptor_pool_info, nullptr,
//&_descriptor_pool);
//	}
//
//	void setup_descriptor_set_layout()
//	{
//		std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings;
//		// Binding 0 : Particle color map
//		set_layout_bindings.push_back(vks::initializers::descriptorSetLayoutBinding(
//			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//			VK_SHADER_STAGE_FRAGMENT_BIT,
//			0));
//		// Binding 1 : Particle gradient ramp
//		set_layout_bindings.push_back(vks::initializers::descriptorSetLayoutBinding(
//			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//			VK_SHADER_STAGE_FRAGMENT_BIT,
//			1));
//
//		VkDescriptorSetLayoutCreateInfo descriptor_layout =
//			vks::initializers::descriptor_set_layout_create_info(
//				set_layout_bindings.data(),
//				static_cast<uint32_t>(set_layout_bindings.size()));
//
//		vkCreateDescriptorSetLayout(_device, &descriptorLayout, nullptr,
//&_graphics._descriptor_set_layout);
//
//		VkPipelineLayoutCreateInfo pipeline_layout_create_info =
//			vks::initializers::pipeline_layout_create_info(
//				&_graphics._descriptor_set_layout,
//				1);
//
//		vkCreatePipelineLayout(_device, &pipeline_layout_create_info, nullptr,
//&_graphics._pipeline_layout);
//	}
//
//	void setup_descriptor_set()
//	{
//		VkDescriptorSetAllocateInfo alloc_info =
//			vks::initializers::descriptor_set_allocate_info(
//				_descriptor_pool,
//				&_graphics._descriptor_set_layout,
//				1);
//
//		vkAllocateDescriptorSets(_device, &allocInfo,
//&_graphics._descriptor_set);
//
//		std::vector<VkWriteDescriptorSet> write_descriptor_sets;
//		// Binding 0 : Particle color map
//		write_descriptor_sets.push_back(vks::initializers::write_descriptor_set(
//			_graphics._descriptor_set,
//			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//			0,
//			&textures._particle._descriptor));
//		// Binding 1 : Particle gradient ramp
//		write_descriptor_sets.push_back(vks::initializers::write_descriptor_set(
//			_graphics._descriptor_set,
//			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
//			1,
//			&textures._gradient._descriptor));
//
//		vkUpdateDescriptorSets(_device,
// static_cast<uint32_t>(write_descriptor_sets.size()),
// write_descriptor_sets.data(), 0, NULL);
//	}
//
//	void prepare_pipelines()
//	{
//		VkPipelineInputAssemblyStateCreateInfo input_assembly_state =
//			vks::initializers::pipeline_input_assembly_state_create_info(
//				VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
//				0,
//				VK_FALSE);
//
//		VkPipelineRasterizationStateCreateInfo rasterization_state =
//			vks::initializers::pipeline_rasterization_state_create_info(
//				VK_POLYGON_MODE_FILL,
//				VK_CULL_MODE_NONE,
//				VK_FRONT_FACE_COUNTER_CLOCKWISE,
//				0);
//
//		VkPipelineColorBlendAttachmentState blend_attachment_state =
//			vks::initializers::pipeline_color_blend_attachment_state(
//				0xf,
//				VK_FALSE);
//
//		VkPipelineColorBlendStateCreateInfo color_blend_state =
//			vks::initializers::pipeline_color_blend_state_create_info(
//				1,
//				&blendAttachmentState);
//
//		VkPipelineDepthStencilStateCreateInfo depth_stencil_state =
//			vks::initializers::pipeline_depth_stencil_state_create_info(
//				VK_FALSE,
//				VK_FALSE,
//				VK_COMPARE_OP_ALWAYS);
//
//		VkPipelineViewportStateCreateInfo viewport_state =
//			vks::initializers::pipeline_viewport_state_create_info(1, 1, 0);
//
//		VkPipelineMultisampleStateCreateInfo multisample_state =
//			vks::initializers::pipeline_multisample_state_create_info(
//				VK_SAMPLE_COUNT_1_BIT,
//				0);
//
//		std::vector<VkDynamicState> dynamic_state_enables = {
//			VK_DYNAMIC_STATE_VIEWPORT,
//			VK_DYNAMIC_STATE_SCISSOR
//		};
//		VkPipelineDynamicStateCreateInfo dynamic_state =
//			vks::initializers::pipeline_dynamic_state_create_info(
//				dynamicStateEnables.data(),
//				static_cast<uint32_t>(dynamicStateEnables.size()),
//				0);
//
//		// Rendering pipeline
//		// Load shaders
//		std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages;
//
//		shaderStages[0] = load_shader(getAssetPath() +
//"shaders/computeparticles/particle.vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
//		shaderStages[1] = load_shader(getAssetPath() +
//"shaders/computeparticles/particle.frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);
//
//		VkGraphicsPipelineCreateInfo pipeline_create_info =
//			vks::initializers::pipeline_create_info(
//				_graphics._pipeline_layout,
//				_render_pass,
//				0);
//
//		pipeline_create_info.pVertexInputState = &vertices._input_state;
//		pipeline_create_info.pInputAssemblyState = &inputAssemblyState;
//		pipeline_create_info.pRasterizationState = &rasterizationState;
//		pipeline_create_info.pColorBlendState = &colorBlendState;
//		pipeline_create_info.pMultisampleState = &multisampleState;
//		pipeline_create_info.pViewportState = &viewportState;
//		pipeline_create_info.pDepthStencilState = &depthStencilState;
//		pipeline_create_info.pDynamicState = &dynamicState;
//		pipeline_create_info.stageCount =
// static_cast<uint32_t>(shaderStages.size()); 		pipeline_create_info.pStages
// = shader_stages.data(); 		pipeline_create_info.renderPass = _render_pass;
//
//		// Additive blending
//		blendAttachmentState.colorWriteMask = 0xF;
//		blendAttachmentState.blendEnable = VK_TRUE;
//		blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
//		blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
//		blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
//		blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
//		blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
//		blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
//
//		vkCreateGraphicsPipelines(_device, _pipeline_cache, 1,
//&pipeline_create_info, nullptr, &_graphics._pipeline);
//	}
//
//	void prepare_compute()
//	{
//		// Create a compute capable device queue
//		// The VulkanDevice::createLogicalDevice functions finds a compute
// capable queue and prefers queue families that only support compute
//		// Depending on the implementation this may result in different queue
// family indices for graphics and computes,
//		// requiring proper synchronization (see the memory barriers in
// build_compute_command_buffer) 		vkGetDeviceQueue(_device,
//_vulkan_device->_queue_family_indices._compute, 0, &_compute._queue);
//
//		// Create compute pipeline
//		// Compute pipelines are created separate from graphics pipelines even
// if they use the same queue (family index)
//
//		std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings = {
//			// Binding 0 : Particle position storage buffer
//			vks::initializers::descriptorSetLayoutBinding(
//				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
//				VK_SHADER_STAGE_COMPUTE_BIT,
//				0),
//			// Binding 1 : Uniform buffer
//			vks::initializers::descriptorSetLayoutBinding(
//				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//				VK_SHADER_STAGE_COMPUTE_BIT,
//				1),
//		};
//
//		VkDescriptorSetLayoutCreateInfo descriptor_layout =
//			vks::initializers::descriptor_set_layout_create_info(
//				set_layout_bindings.data(),
//				static_cast<uint32_t>(set_layout_bindings.size()));
//
//		vkCreateDescriptorSetLayout(_device, &descriptorLayout, nullptr,
//&_compute._descriptor_set_layout);
//
//		VkPipelineLayoutCreateInfo p_pipeline_layout_create_info =
//			vks::initializers::pipeline_layout_create_info(
//				&_compute._descriptor_set_layout,
//				1);
//
//		vkCreatePipelineLayout(_device, &p_pipeline_layout_create_info, nullptr,
//&_compute._pipeline_layout);
//
//		VkDescriptorSetAllocateInfo alloc_info =
//			vks::initializers::descriptor_set_allocate_info(
//				_descriptor_pool,
//				&_compute._descriptor_set_layout,
//				1);
//
//		vkAllocateDescriptorSets(_device, &allocInfo,
//&_compute._descriptor_set);
//
//		std::vector<VkWriteDescriptorSet> compute_write_descriptor_sets =
//		{
//			// Binding 0 : Particle position storage buffer
//			vks::initializers::write_descriptor_set(
//				_compute._descriptor_set,
//				VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
//				0,
//				&_compute._storage_buffer._descriptor),
//			// Binding 1 : Uniform buffer
//			vks::initializers::write_descriptor_set(
//				_compute._descriptor_set,
//				VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
//				1,
//				&_compute._uniform_buffer._descriptor)
//		};
//
//		vkUpdateDescriptorSets(_device,
// static_cast<uint32_t>(computeWriteDescriptorSets.size()),
// compute_write_descriptor_sets.data(), 0, NULL);
//
//		// Create pipeline
//		VkComputePipelineCreateInfo compute_pipeline_create_info =
// vks::initializers::compute_pipeline_create_info(_compute._pipeline_layout,
// 0); 		compute_pipeline_create_info.stage = load_shader(getAssetPath() +
//"shaders/computeparticles/particle.comp.spv", VK_SHADER_STAGE_COMPUTE_BIT);
//		vkCreateComputePipelines(_device, _pipeline_cache, 1,
//&compute_pipeline_create_info, nullptr, &_compute._pipeline);
//
//		// Separate command pool as queue family for compute may be different
// than graphics 		VkCommandPoolCreateInfo cmd_pool_info = {};
// cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
// cmdPoolInfo.queueFamilyIndex =
// _vulkan_device->_queue_family_indices._compute;
// cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
// vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &_compute._command_pool);
//
//		// Create a command buffer for compute operations
//		VkCommandBufferAllocateInfo cmd_buf_allocate_info =
//			vks::initializers::command_buffer_allocate_info(
//				_compute._command_pool,
//				VK_COMMAND_BUFFER_LEVEL_PRIMARY,
//				1);
//
//		vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo,
//&_compute._command_buffer);
//
//		// Fence for compute CB sync
//		VkFenceCreateInfo fence_create_info =
// vks::initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
//		vkCreateFence(_device, &fenceCreateInfo, nullptr, &_compute._fence);
//
//		// Build a single command buffer containing the compute dispatch
// commands 		buildComputeCommandBuffer();
//	}
//
//	// Prepare and initialize uniform buffer containing shader uniforms
//	void prepare_uniform_buffers()
//	{
//		// Compute shader uniform buffer block
//		_vulkan_device->createBuffer(
//			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
// VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
//			&_compute._uniform_buffer,
//			sizeof(_compute._ubo));
//
//		// Map for host access
//		_compute._uniform_buffer.map();
//
//		updateUniformBuffers();
//	}
//
//	void update_uniform_buffers()
//	{
//		_compute._ubo._delta_t = _frame_timer * 2.5f;
//		_compute._ubo._dest_x = sin(glm::radians(_timer * 360.0f)) * 0.75f;
//		_compute._ubo._dest_y = 0.0f;
//
//		memcpy(_compute._uniform_buffer._mapped, &_compute._ubo,
// sizeof(_compute._ubo));
//	}
//
//	void draw()
//	{
//		VkSubmitInfo compute_submit_info = vks::initializers::submitInfo();
//		computeSubmitInfo.commandBufferCount = 1;
//		computeSubmitInfo.pCommandBuffers = &_compute._command_buffer;
//
//		vkQueueSubmit(_compute._queue, 1, &computeSubmitInfo, _compute._fence);
//
//		// Submit graphics commands
//		VulkanExamplePortBase::prepareFrame();
//
//		_submit_info.commandBufferCount = 1;
//		_submit_info.pCommandBuffers = &_draw_cmd_buffers[_current_buffer];
//		vkQueueSubmit(_queue, 1, &_submit_info, VK_NULL_HANDLE);
//
//		VulkanExamplePortBase::submitFrame();
//
//		// Submit compute commands
//		vkWaitForFences(_device, 1, &_compute._fence, VK_TRUE, UINT64_MAX);
//		vkResetFences(_device, 1, &_compute._fence);
//	}
//
//	void prepare()
//	{
//		VulkanExamplePortBase::prepare();
//		loadAssets();
//		prepareStorageBuffers();
//		prepareUniformBuffers();
//		setupDescriptorSetLayout();
//		preparePipelines();
//		setupDescriptorPool();
//		setupDescriptorSet();
//		prepareCompute();
//		buildCommandBuffers();
//		_prepared = true;
//	}
//
//	virtual void render()
//	{
//		if (!_prepared)
//			return;
//		draw();
//
//		if (_anim_start > 0.0f)
//		{
//			_anim_start -= _frame_timer * 5.0f;
//		}
//		else if (_anim_start <= 0.0f)
//		{
//			_timer += _frame_timer * 0.04f;
//			if (_timer > 1.f)
//				_timer = 0.f;
//		}
//
//		updateUniformBuffers();
//	}
//
//};
//
// VulkanExample *vulkanExample;
// LRESULT CALLBACK WndProc(HWND h_wnd, UINT u_msg, WPARAM w_param, LPARAM
// l_param)
//{
//	if (vulkanExample != NULL)
//	{
//		vulkanExample->handleMessages(hWnd, u_msg, w_param, l_param);
//	}
//	return (DefWindowProc(hWnd, u_msg, w_param, l_param));
//}
//
//
// void port_example()
//{
//	HINSTANCE h_instance = GetModuleHandle(NULL);
//	vulkanExample = new VulkanExample();
//	vulkanExample->initVulkan();
//	vulkanExample->setupWindow(hInstance, WndProc);
//	vulkanExample->prepare();
//	vulkanExample->renderLoop();
//	delete(vulkanExample);
//}
//
// VkResult VulkanExamplePortBase::createInstance(bool enable_validation)
//{
//	_settings._validation = enable_validation;
//
//	// Validation can also be forced via a define
//#if defined(_VALIDATION)
//	_settings._validation = true;
//#endif
//
//	VkApplicationInfo app_info = {};
//	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//	appInfo.pApplicationName = _name.c_str();
//	appInfo.pEngineName = _name.c_str();
//	appInfo.apiVersion = _api_version;
//
//	std::vector<const char*> instance_extensions = {
// VK_KHR_SURFACE_EXTENSION_NAME };
//
//	// Enable surface extensions depending on os
//#if defined(_WIN32)
//	instanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
//#elif defined(VK_USE_PLATFORM_ANDROID_KHR)
//	instanceExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
//#elif defined(_DIRECT2DISPLAY)
//	instanceExtensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
//#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
//	instanceExtensions.push_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
//#elif defined(VK_USE_PLATFORM_XCB_KHR)
//	instanceExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
//#elif defined(VK_USE_PLATFORM_IOS_MVK)
//	instanceExtensions.push_back(VK_MVK_IOS_SURFACE_EXTENSION_NAME);
//#elif defined(VK_USE_PLATFORM_MACOS_MVK)
//	instanceExtensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
//#endif
//
//	if (_enabled_instance_extensions.size() > 0) {
//		for (auto enabled_extension : _enabled_instance_extensions) {
//			instanceExtensions.push_back(enabledExtension);
//		}
//	}
//
//	VkInstanceCreateInfo instance_create_info = {};
//	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//	instanceCreateInfo.pNext = NULL;
//	instanceCreateInfo.pApplicationInfo = &appInfo;
//	if (instanceExtensions.size() > 0)
//	{
//		if (_settings._validation)
//		{
//			instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
//		}
//		instanceCreateInfo.enabledExtensionCount =
//(uint32_t)instanceExtensions.size();
//		instanceCreateInfo.ppEnabledExtensionNames = instance_extensions.data();
//	}
//
//	return vk_create_instance(&instanceCreateInfo, nullptr, &_instance);
//}
//
// std::string VulkanExamplePortBase::getWindowTitle()
//{
//	std::string device(_device_properties.deviceName);
//	std::string window_title;
//	windowTitle = _title + " - " + device;
//	return window_title;
//}
//
//#if !(defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
//// i_o_s & mac_o_s: VulkanExamplePortBase::getAssetPath() implemented
/// externally to allow access to Objective-C components
// const std::string VulkanExamplePortBase::getAssetPath()
//{
//#if defined(VK_USE_PLATFORM_ANDROID_KHR)
//	return "";
//#elif defined(VK_EXAMPLE_DATA_DIR)
//	return VK_EXAMPLE_DATA_DIR;
//#else
//	return "hack_port_example/";
//#endif
//}
//#endif
//
// void VulkanExamplePortBase::createCommandBuffers()
//{
//	// Create one command buffer for each swap chain image and reuse for
// rendering 	_draw_cmd_buffers.resize(_swap_chain._image_count);
//
//	VkCommandBufferAllocateInfo cmd_buf_allocate_info =
//		vks::initializers::command_buffer_allocate_info(
//			_cmd_pool,
//			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
//			static_cast<uint32_t>(_draw_cmd_buffers.size()));
//
//	vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo,
//_draw_cmd_buffers.data());
//}
//
// void VulkanExamplePortBase::destroyCommandBuffers()
//{
//	vkFreeCommandBuffers(_device, _cmd_pool,
// static_cast<uint32_t>(_draw_cmd_buffers.size()), _draw_cmd_buffers.data());
//}
//
// VkCommandBuffer
// VulkanExamplePortBase::createCommandBuffer(VkCommandBufferLevel level, bool
// begin)
//{
//	VkCommandBuffer cmd_buffer;
//
//	VkCommandBufferAllocateInfo cmd_buf_allocate_info =
//		vks::initializers::command_buffer_allocate_info(
//			_cmd_pool,
//			level,
//			1);
//
//	vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &cmdBuffer);
//
//	// If requested, also start the new command buffer
//	if (begin)
//	{
//		VkCommandBufferBeginInfo cmd_buf_info =
// vks::initializers::commandBufferBeginInfo();
// vkBeginCommandBuffer(cmdBuffer, &cmdBufInfo);
//	}
//
//	return cmd_buffer;
//}
//
// void VulkanExamplePortBase::flushCommandBuffer(VkCommandBuffer
// command_buffer, VkQueue queue, bool free)
//{
//	if (commandBuffer == VK_NULL_HANDLE)
//	{
//		return;
//	}
//
//	vkEndCommandBuffer(commandBuffer);
//
//	VkSubmitInfo submit_info = {};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	submitInfo.commandBufferCount = 1;
//	submitInfo.pCommandBuffers = &commandBuffer;
//
//	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(queue);
//
//	if (free)
//	{
//		vkFreeCommandBuffers(_device, _cmd_pool, 1, &commandBuffer);
//	}
//}
//
// void VulkanExamplePortBase::createPipelineCache()
//{
//	VkPipelineCacheCreateInfo pipeline_cache_create_info = {};
//	pipelineCacheCreateInfo.sType =
// VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
// vkCreatePipelineCache(_device, &pipelineCacheCreateInfo, nullptr,
//&_pipeline_cache);
//}
//
// void VulkanExamplePortBase::prepare()
//{
//	initSwapchain();
//	createCommandPool();
//	setupSwapChain();
//	createCommandBuffers();
//	createSynchronizationPrimitives();
//	setupDepthStencil();
//	setupRenderPass();
//	createPipelineCache();
//	setupFrameBuffer();
//}
//
// VkPipelineShaderStageCreateInfo VulkanExamplePortBase::loadShader(std::string
// file_name, VkShaderStageFlagBits stage)
//{
//	VkPipelineShaderStageCreateInfo shader_stage = {};
//	shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	shaderStage.stage = stage;
//
//	shaderStage.module = tools::loadShader(file_name.c_str(), _device);
//	shaderStage.pName = "main"; // todo : make param
//	assert(shaderStage.module != VK_NULL_HANDLE);
//	_shader_modules.push_back(shaderStage.module);
//	return shader_stage;
//}
//
// void VulkanExamplePortBase::renderFrame()
//{
//	auto t_start = std::chrono::high_resolution_clock::now();
//
//	render();
//	_frame_counter++;
//	auto t_end = std::chrono::high_resolution_clock::now();
//	auto t_diff = std::chrono::duration<double, std::milli>(tEnd -
// t_start).count(); 	_frame_timer = (float)tDiff / 1000.0f;
//	_camera.update(_frame_timer);
//	if (_camera.moving())
//	{
//		_view_updated = true;
//	}
//	// Convert to clamped timer value
//	if (!_paused)
//	{
//		_timer += _timer_speed * _frame_timer;
//		if (_timer > 1.0)
//		{
//			_timer -= 1.0f;
//		}
//	}
//	float fps_timer = (float)std::chrono::duration<double, std::milli>(tEnd -
//_last_timestamp).count(); 	if (fpsTimer > 1000.0f)
//	{
//		_last_f_p_s = static_cast<uint32_t>((float)_frame_counter * (1000.0f /
// fps_timer)); 		_frame_counter = 0; 		_last_timestamp = t_end;
//	}
//}
//
// void VulkanExamplePortBase::renderLoop()
//{
//	if (_benchmark._active) {
//		_benchmark.run([=] { render(); }, _vulkan_device->_properties);
//		vkDeviceWaitIdle(_device);
//		if (_benchmark._file_name != "") {
//			_benchmark.saveResults();
//		}
//		return;
//	}
//
//	_dest_width = _width;
//	_dest_height = _height;
//	_last_timestamp = std::chrono::high_resolution_clock::now();
//
//	MSG msg;
//	bool quit_message_received = false;
//	while (!quitMessageReceived) {
//		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//			if (msg.message == WM_QUIT) {
//				quitMessageReceived = true;
//				break;
//			}
//		}
//		if (!IsIconic(_window)) {
//			renderFrame();
//		}
//	}
//	// Flush device to make sure all resources can be freed
//	if (_device != VK_NULL_HANDLE) {
//		vkDeviceWaitIdle(_device);
//	}
//}
//
// void VulkanExamplePortBase::prepareFrame()
//{
//	// Acquire the next image from the swap chain
//	_swap_chain.acquireNextImage(_semaphores._present_complete,
//&_current_buffer);
//
//}
//
// void VulkanExamplePortBase::submitFrame()
//{
//	_swap_chain.queuePresent(_queue, _current_buffer,
//_semaphores._render_complete);
//
//	vkQueueWaitIdle(_queue);
//}
//
// VulkanExamplePortBase::VulkanExamplePortBase(bool enable_validation)
//{
//	// Check for a valid asset path
//	struct stat info;
//	if (stat(getAssetPath().c_str(), &info) != 0)
//	{
//		std::string msg = "Could not locate asset path in \"" + get_asset_path()
//+
//"\" !"; 		exit(-1);
//	}
//
//	_settings._validation = enable_validation;
//}
//
// VulkanExamplePortBase::~VulkanExamplePortBase()
//{
//	// Clean up Vulkan resources
//	_swap_chain.cleanup();
//	if (_descriptor_pool != VK_NULL_HANDLE)
//	{
//		vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
//	}
//	destroyCommandBuffers();
//	vkDestroyRenderPass(_device, _render_pass, nullptr);
//	for (uint32_t i = 0; i < _frame_buffers.size(); i++)
//	{
//		vkDestroyFramebuffer(_device, _frame_buffers[i], nullptr);
//	}
//
//	for (auto& shader_module : _shader_modules)
//	{
//		vkDestroyShaderModule(_device, shader_module, nullptr);
//	}
//	vkDestroyImageView(_device, _depth_stencil._view, nullptr);
//	vkDestroyImage(_device, _depth_stencil._image, nullptr);
//	vkFreeMemory(_device, _depth_stencil._mem, nullptr);
//
//	vkDestroyPipelineCache(_device, _pipeline_cache, nullptr);
//
//	vkDestroyCommandPool(_device, _cmd_pool, nullptr);
//
//	vkDestroySemaphore(_device, _semaphores._present_complete, nullptr);
//	vkDestroySemaphore(_device, _semaphores._render_complete, nullptr);
//	for (auto& fence : _wait_fences) {
//		vkDestroyFence(_device, fence, nullptr);
//	}
//
//	delete _vulkan_device;
//
//	vkDestroyInstance(_instance, nullptr);
//}
//
// bool VulkanExamplePortBase::initVulkan()
//{
//	VkResult err;
//
//	// Vulkan instance
//	err = create_instance(_settings._validation);
//	if (err) {
//		tools::exitFatal("Could not create Vulkan instance : \n" +
// tools::errorString(err), err); 		return false;
//	}
//
//#if defined(VK_USE_PLATFORM_ANDROID_KHR)
//	vks::android::loadVulkanFunctions(instance);
//#endif
//
//	// Physical device
//	uint32_t gpu_count = 0;
//	// Get number of available physical devices
//	vkEnumeratePhysicalDevices(_instance, &gpuCount, nullptr);
//	assert(gpuCount > 0);
//	// Enumerate devices
//	std::vector<VkPhysicalDevice> physical_devices(gpuCount);
//	err = vk_enumerate_physical_devices(_instance, &gpuCount,
// physical_devices.data()); 	if (err) { 		tools::exitFatal("Could not
// enumerate physical devices : \n" + tools::errorString(err), err); return
// false;
//	}
//
//	// GPU selection
//
//	// Select physical device to be used for the Vulkan example
//	// Defaults to the first device unless specified by command line
//
//	_physical_device = physical_devices[0];
//
//	// Store properties (including limits), features and memory properties of
// the phyiscal device (so that examples can check against them)
//	vkGetPhysicalDeviceProperties(_physical_device, &_device_properties);
//	vkGetPhysicalDeviceFeatures(_physical_device, &_device_features);
//	vkGetPhysicalDeviceMemoryProperties(_physical_device,
//&_device_memory_properties);
//
//	// Derived examples can override this to set actual features (based on above
// readings) to enable for logical device creation 	getEnabledFeatures();
//
//	// Vulkan device creation
//	// This is handled by a separate class that gets a logical device
// representation
//	// and encapsulates functions related to a device
//	_vulkan_device = new vks::VulkanDevice(_physical_device);
//	VkResult res = _vulkan_device->createLogicalDevice(_enabled_features,
//_enabled_device_extensions); 	if (res != VK_SUCCESS) {
// tools::exitFatal("Could not create Vulkan device: \n" +
// tools::errorString(res), res); 		return false;
//	}
//	_device = _vulkan_device->_logical_device;
//
//	// Get a graphics queue from the device
//	vkGetDeviceQueue(_device, _vulkan_device->_queue_family_indices._graphics,
// 0, &_queue);
//
//	// Find a suitable depth format
//	VkBool32 valid_depth_format =
// tools::getSupportedDepthFormat(_physical_device, &_depth_format);
//	assert(validDepthFormat);
//
//	_swap_chain.connect(_instance, _physical_device, _device);
//
//	// Create synchronization objects
//	VkSemaphoreCreateInfo semaphore_create_info =
// vks::initializers::semaphoreCreateInfo();
//	// Create a semaphore used to synchronize image presentation
//	// Ensures that the image is displayed before we start submitting new
// commands to the queu 	vkCreateSemaphore(_device, &semaphoreCreateInfo,
// nullptr,
//&_semaphores._present_complete);
//	// Create a semaphore used to synchronize command submission
//	// Ensures that the image is not presented until all commands have been
// sumbitted and executed 	vkCreateSemaphore(_device, &semaphoreCreateInfo,
// nullptr, &_semaphores._render_complete);
//
//	// Set up submit info structure
//	// Semaphores will stay the same during application lifetime
//	// Command buffer submission info is set by each example
//	_submit_info = vks::initializers::submitInfo();
//	_submit_info.pWaitDstStageMask = &_submit_pipeline_stages;
//	_submit_info.waitSemaphoreCount = 1;
//	_submit_info.pWaitSemaphores = &_semaphores._present_complete;
//	_submit_info.signalSemaphoreCount = 1;
//	_submit_info.pSignalSemaphores = &_semaphores._render_complete;
//
//
//	return true;
//}
//
//#if defined(_WIN32)
//
//#include <strsafe.h>
// HWND VulkanExamplePortBase::setupWindow(HINSTANCE hinstance, WNDPROC wndproc)
//{
//	_window_instance = hinstance;
//
//	WNDCLASSEX wnd_class;
//
//	wndClass.cbSize = sizeof(WNDCLASSEX);
//	wndClass.style = CS_HREDRAW | CS_VREDRAW;
//	wndClass.lpfnWndProc = wndproc;
//	wndClass.cbClsExtra = 0;
//	wndClass.cbWndExtra = 0;
//	wndClass.hInstance = hinstance;
//	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
//	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
//	wndClass.lpszMenuName = NULL;
//	wndClass.lpszClassName = "vulkanExample";
//	wndClass.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
//
//	if (!RegisterClassEx(&wndClass))
//	{
//		std::cout << "Could not register window class!\n";
//		fflush(stdout);
//		exit(1);
//	}
//
//	int screen_width = GetSystemMetrics(SM_CXSCREEN);
//	int screen_height = GetSystemMetrics(SM_CYSCREEN);
//
//	if (_settings._fullscreen)
//	{
//		DEVMODE dm_screen_settings;
//		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
//		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
//		dmScreenSettings.dmPelsWidth = screen_width;
//		dmScreenSettings.dmPelsHeight = screen_height;
//		dmScreenSettings.dmBitsPerPel = 32;
//		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH |
// DM_PELSHEIGHT;
//
//		if ((_width != (uint32_t)screenWidth) && (_height !=
//(uint32_t)screenHeight))
//		{
//			if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) !=
// DISP_CHANGE_SUCCESSFUL)
//			{
//				_settings._fullscreen = false;
//			}
//		}
//
//	}
//
//	DWORD dw_ex_style;
//	DWORD dw_style;
//
//	if (_settings._fullscreen)
//	{
//		dwExStyle = WS_EX_APPWINDOW;
//		dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
//	}
//	else
//	{
//		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
//		dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
//	}
//
//	RECT window_rect;
//	windowRect.left = 0L;
//	windowRect.top = 0L;
//	windowRect.right = _settings._fullscreen ? (long)screenWidth : (long)_width;
//	windowRect.bottom = _settings._fullscreen ? (long)screenHeight :
//(long)_height;
//
//	AdjustWindowRectEx(&windowRect, dw_style, FALSE, dw_ex_style);
//
//	std::string window_title = get_window_title();
//	_window = CreateWindowEx(0,
//		"vulkanExample",
//		"TestWindow",
//		dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
//		0,
//		0,
//		windowRect.right - window_rect.left,
//		windowRect.bottom - window_rect.top,
//		NULL,
//		NULL,
//		hinstance,
//		NULL);
//
//	if (!_window)
//	{
//		LPVOID lp_msg_buf;
//		LPVOID lp_display_buf;
//		DWORD dw = GetLastError();
//		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
// FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dw,
// MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
// lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, (lstrlen((LPCTSTR)lpMsgBuf)
// +
// 120 + 40) * sizeof(TCHAR)); 		StringCchPrintf((LPTSTR)lpDisplayBuf,
// LocalSize(lpDisplayBuf)
/// sizeof(TCHAR), TEXT("%s failed with error %d: %s"), 0, dw, lp_msg_buf);
/// auto
// message = (LPCTSTR)lpDisplayBuf; 		std::cout << message << std::endl;
//		LocalFree(lpMsgBuf);
//		LocalFree(lpDisplayBuf);
//
//		// Display the error message and exit the process
//
//		printf("Could not create window!\n");
//		fflush(stdout);
//		return nullptr;
//	}
//
//	if (!_settings._fullscreen)
//	{
//		// Center on screen
//		uint32_t x = (GetSystemMetrics(SM_CXSCREEN) - window_rect.right) / 2;
//		uint32_t y = (GetSystemMetrics(SM_CYSCREEN) - window_rect.bottom) / 2;
//		SetWindowPos(_window, 0, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
//	}
//
//	ShowWindow(_window, SW_SHOW);
//	SetForegroundWindow(_window);
//	SetFocus(_window);
//
//	return _window;
//}
//
// void VulkanExamplePortBase::handleMessages(HWND h_wnd, UINT u_msg, WPARAM
// w_param, LPARAM l_param)
//{
//	switch (uMsg)
//	{
//	case WM_CLOSE:
//		_prepared = false;
//		DestroyWindow(hWnd);
//		PostQuitMessage(0);
//		break;
//	case WM_PAINT:
//		ValidateRect(_window, NULL);
//		break;
//	case WM_KEYUP:
//		break;
//	case WM_SIZE:
//		if ((_prepared) && (wParam != SIZE_MINIMIZED))
//		{
//			if ((_resizing) || ((wParam == SIZE_MAXIMIZED) || (wParam ==
// SIZE_RESTORED)))
//			{
//				_dest_width = LOWORD(lParam);
//				_dest_height = HIWORD(lParam);
//			}
//		}
//		break;
//	case WM_ENTERSIZEMOVE:
//		_resizing = true;
//		break;
//	case WM_EXITSIZEMOVE:
//		_resizing = false;
//		break;
//	}
//}
//#endif
//
// void VulkanExamplePortBase::createSynchronizationPrimitives()
//{
//	// Wait fences to sync command buffer access
//	VkFenceCreateInfo fence_create_info =
// vks::initializers::fenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
//	_wait_fences.resize(_draw_cmd_buffers.size());
//	for (auto& fence : _wait_fences) {
//		vkCreateFence(_device, &fenceCreateInfo, nullptr, &fence);
//	}
//}
//
// void VulkanExamplePortBase::createCommandPool()
//{
//	VkCommandPoolCreateInfo cmd_pool_info = {};
//	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//	cmdPoolInfo.queueFamilyIndex = _swap_chain._queue_node_index;
//	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
//	vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &_cmd_pool);
//}
//
// void VulkanExamplePortBase::setupDepthStencil()
//{
//	VkImageCreateInfo image_create_info{};
//	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//	image_create_info.imageType = VK_IMAGE_TYPE_2D;
//	image_create_info.format = _depth_format;
//	image_create_info.extent = { _width, _height, 1 };
//	image_create_info.mipLevels = 1;
//	image_create_info.arrayLayers = 1;
//	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
//	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
//	image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
// VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
//
//	vkCreateImage(_device, &image_create_info, nullptr, &_depth_stencil._image);
//	VkMemoryRequirements mem_reqs{};
//	vkGetImageMemoryRequirements(_device, _depth_stencil._image, &memReqs);
//
//	VkMemoryAllocateInfo mem_allloc{};
//	memAllloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//	memAllloc.allocationSize = mem_reqs.size;
//	memAllloc.memoryTypeIndex =
//_vulkan_device->getMemoryType(memReqs.memoryTypeBits,
// VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT); 	vkAllocateMemory(_device,
// &memAllloc, nullptr, &_depth_stencil._mem); 	vkBindImageMemory(_device,
//_depth_stencil._image, _depth_stencil._mem, 0);
//
//	VkImageViewCreateInfo image_view_c_i{};
//	imageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	imageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
//	imageViewCI.image = _depth_stencil._image;
//	imageViewCI.format = _depth_format;
//	imageViewCI.subresourceRange.baseMipLevel = 0;
//	imageViewCI.subresourceRange.levelCount = 1;
//	imageViewCI.subresourceRange.baseArrayLayer = 0;
//	imageViewCI.subresourceRange.layerCount = 1;
//	imageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//	// Stencil aspect should only be set on depth + stencil formats
//(VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT 	if
//(_depth_format
//>= VK_FORMAT_D16_UNORM_S8_UINT) {
// imageViewCI.subresourceRange.aspectMask
//|= VK_IMAGE_ASPECT_STENCIL_BIT;
//	}
//	vkCreateImageView(_device, &imageViewCI, nullptr, &_depth_stencil._view);
//}
//
// void VulkanExamplePortBase::setupFrameBuffer()
//{
//	VkImageView attachments[2];
//
//	// Depth/Stencil attachment is the same for all frame buffers
//	attachments[1] = _depth_stencil._view;
//
//	VkFramebufferCreateInfo frame_buffer_create_info = {};
//	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//	frameBufferCreateInfo.pNext = NULL;
//	frameBufferCreateInfo.renderPass = _render_pass;
//	frameBufferCreateInfo.attachmentCount = 2;
//	frameBufferCreateInfo.pAttachments = attachments;
//	frameBufferCreateInfo.width = _width;
//	frameBufferCreateInfo.height = _height;
//	frameBufferCreateInfo.layers = 1;
//
//	// Create frame buffers for every swap chain image
//	_frame_buffers.resize(_swap_chain._image_count);
//	for (uint32_t i = 0; i < _frame_buffers.size(); i++)
//	{
//		attachments[0] = _swap_chain._buffers[i]._view;
//		vkCreateFramebuffer(_device, &frameBufferCreateInfo, nullptr,
//&_frame_buffers[i]);
//	}
//}
//
// void VulkanExamplePortBase::setupRenderPass()
//{
//	std::array<VkAttachmentDescription, 2> attachments = {};
//	// Color attachment
//	attachments[0].format = _swap_chain._color_format;
//	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
//	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//	// Depth attachment
//	attachments[1].format = _depth_format;
//	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
//	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//	attachments[1].finalLayout =
// VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//	VkAttachmentReference color_reference = {};
//	colorReference.attachment = 0;
//	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//	VkAttachmentReference depth_reference = {};
//	depthReference.attachment = 1;
//	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//	VkSubpassDescription subpass_description = {};
//	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//	subpassDescription.colorAttachmentCount = 1;
//	subpassDescription.pColorAttachments = &colorReference;
//	subpassDescription.pDepthStencilAttachment = &depthReference;
//	subpassDescription.inputAttachmentCount = 0;
//	subpassDescription.pInputAttachments = nullptr;
//	subpassDescription.preserveAttachmentCount = 0;
//	subpassDescription.pPreserveAttachments = nullptr;
//	subpassDescription.pResolveAttachments = nullptr;
//
//	// Subpass dependencies for layout transitions
//	std::array<VkSubpassDependency, 2> dependencies;
//
//	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
//	dependencies[0].dstSubpass = 0;
//	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
//	dependencies[0].dstStageMask =
// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; dependencies[0].srcAccessMask
// = VK_ACCESS_MEMORY_READ_BIT; 	dependencies[0].dstAccessMask =
// VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//
//	dependencies[1].srcSubpass = 0;
//	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
//	dependencies[1].srcStageMask =
// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; 	dependencies[1].dstStageMask
// = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; 	dependencies[1].srcAccessMask =
// VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
//	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//
//	VkRenderPassCreateInfo render_pass_info = {};
//	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//	renderPassInfo.pAttachments = attachments.data();
//	renderPassInfo.subpassCount = 1;
//	renderPassInfo.pSubpasses = &subpassDescription;
//	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
//	renderPassInfo.pDependencies = dependencies.data();
//
//	vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass);
//}
//
// void VulkanExamplePortBase::getEnabledFeatures()
//{
//	// Can be overriden in derived class
//}
//
// void VulkanExamplePortBase::initSwapchain()
//{
//	_swap_chain.initSurface(_window_instance, _window);
//}
//
// void VulkanExamplePortBase::setupSwapChain()
//{
//	_swap_chain.create(&_width, &_height, _settings._vsync);
//}