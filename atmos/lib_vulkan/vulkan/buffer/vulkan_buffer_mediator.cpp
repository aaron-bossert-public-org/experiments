
#include <vulkan/buffer/vulkan_buffer_mediator.h>

#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/buffer/vulkan_buffer.h>
#include <vulkan/buffer/vulkan_fence.h>
#include <vulkan/buffer/vulkan_queue.h>
#include <vulkan/buffer/vulkan_command_buffer.h>


using namespace igpu;

namespace
{
	// allocate/build/execute/free a temporary command buffer before function exits
	template<typename CommandBuilder, typename... Args>
	void submit_sync(
		vulkan_queue & queue,
		CommandBuilder&& builder)
	{
		vulkan_command_buffer command_buffer({
			queue.cfg().device,
			queue.command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			});

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		
		auto raw_command_buffer = command_buffer.get();
		vkBeginCommandBuffer(raw_command_buffer, &begin_info);
		builder(raw_command_buffer);
		vkEndCommandBuffer(raw_command_buffer);



		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &raw_command_buffer;

		vkQueueSubmit(queue.get(), 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue.get());
	}

	// allocate/build/submit command buffer to be executed asynnchronously
	template<typename CommandBuilder>
	const scoped_ptr < vulkan_fence >&
		submit(
			vulkan_queue& queue,
			CommandBuilder&& command_builder)
	{
		queue.free_completed_commands();

		vulkan_command_buffer::config cfg = {
			queue.cfg().device,
			queue.command_pool(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		};

		queue.pending_commands().emplace_back(
			cfg,
			vulkan_fence::make({ queue.cfg().device })
		);

		vulkan_command_buffer& command_buffer = queue.pending_commands().back();
		VkCommandBuffer raw_command_buffer = command_buffer.get();
		VkCommandBufferBeginInfo begin_info = {};

		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(raw_command_buffer, &begin_info);

		command_builder(raw_command_buffer);
		vkEndCommandBuffer(raw_command_buffer);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &raw_command_buffer;

		vkQueueSubmit(queue.cfg().queue, 1, &submit_info, command_buffer.fence()->get());

		return command_buffer.fence();
	}

	// allocate/build/execute/free a temporary command buffer before function exits
	// ensure memory/execution dependencies are added for args
	template<typename CommandBuilder, typename... Args>
	void submit_sync(
		const scoped_ptr < vulkan_queue >& queue,
		CommandBuilder&& command_builder,
		Args... args)
	{
		submit_sync(
			*queue,
			[&](VkCommandBuffer command_buffer) {

				command_builder(command_buffer,
					add_dependency(command_buffer, args.buffer, { args.access, args.stage, args.dependency, queue })...);

			});
	}

	// allocate/build/submit command buffer to be executed asynnchronously
	// ensure memory/execution dependencies are added for args
	template<typename CommandBuilder, typename... Args>
	const scoped_ptr < vulkan_fence >&
		submit(
			const scoped_ptr < vulkan_queue >& queue,
			CommandBuilder&& command_builder,
			Args... args)
	{
		return submit(
			*queue,
			[&](VkCommandBuffer command_buffer) {

				command_builder(command_buffer,
					add_dependency(command_buffer, args.buffer, { args.access, args.stage, args.dependency, queue })...);

			});
	}

	struct buffer_dependency
	{
		vulkan_buffer& buffer;
		VkAccessFlagBits access;
		VkPipelineStageFlagBits stage;
		VkDependencyFlagBits dependency;
	};
	
	buffer_dependency dependency(
		vulkan_buffer& buffer,
		VkAccessFlagBits access,
		VkPipelineStageFlagBits stage,
		VkDependencyFlagBits dependency = (VkDependencyFlagBits)0)
	{
		return { buffer, access, stage, dependency };
	};

	VkBuffer add_dependency(
		VkCommandBuffer command_buffer,
		vulkan_buffer& buffer,
		const vulkan_buffer::ownership& target_owner)
	{
		const vulkan_buffer::ownership& current_owner = buffer.owner();

		// cannot add dependency with null queue
		if (!target_owner.queue)
		{
			LOG_CRITICAL("target owner queue is null");
		}
		// add dependency to some operation within the queue that has already aquired this buffer
		else if (current_owner.queue == target_owner.queue)
		{
			if (target_owner.access != current_owner.access ||
				target_owner.stage != current_owner.stage)
			{
				// apparently this is slower than VkMemoryBarrier, need an api to handle these in bulk with VkMemoryBarrier.
				VkBufferMemoryBarrier buffer_memory_barrier = {
					VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr,
					(VkFlags)current_owner.access, (VkFlags)target_owner.access,
					VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
					buffer.get(), 0, buffer.cfg().size };

				vkCmdPipelineBarrier(
					command_buffer,
					current_owner.stage, target_owner.stage, target_owner.dependency,
					0, nullptr,
					1, &buffer_memory_barrier,
					0, nullptr);
			}
		}

		// release current queue's ownership of this buffer and aquire ownership on target_queue
		else if (current_owner.queue)
		{
			// Relese exclusive ownersip on _queue
			submit_sync(
				current_owner.queue,
				[&](VkCommandBuffer current_owner_command_buffer) {
					VkBufferMemoryBarrier buffer_memory_barrier = {
						VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr,
						(VkFlags)current_owner.access, 0,
						current_owner.queue->cfg().family_index, target_owner.queue->cfg().family_index,
						buffer.get(), 0, buffer.cfg().size };

					vkCmdPipelineBarrier(
						current_owner_command_buffer,
						current_owner.stage, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, target_owner.dependency,
						0, nullptr,
						1, &buffer_memory_barrier,
						0, nullptr);
				});



			if (buffer.fence() && buffer.fence()->is_ready())
			{
				LOG_CRITICAL("fence is not ready immediately after submit_sync. Is fence (and buffer) still in use?");
			}

			// Aquire exclusive ownership on target_queue

			VkBufferMemoryBarrier buffer_memory_barrier = {
				VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER, nullptr,
				0, (VkFlags)target_owner.access,
				current_owner.queue->cfg().family_index, target_owner.queue->cfg().family_index,
				buffer.get(), 0, buffer.cfg().size };

			vkCmdPipelineBarrier(
				command_buffer,
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, target_owner.stage, target_owner.dependency,
				0, nullptr,
				1, &buffer_memory_barrier,
				0, nullptr);
		}

		buffer.owner(target_owner);

		return buffer.get();
	}
}

const vulkan_buffer_mediator::config& vulkan_buffer_mediator::cfg() const
{
	return _cfg;
}

VmaAllocator vulkan_buffer_mediator::vma()
{
	return _vma;
}

void vulkan_buffer_mediator::copy(
	vulkan_buffer& src,
	vulkan_buffer& dst)
{
	submit_sync(
		_cfg.transfer_queue,
		[&](VkCommandBuffer command_buffer,
			VkBuffer src_buffer,
			VkBuffer dst_buffer) {

				VkBufferCopy region = {};
				region.srcOffset = 0;
				region.dstOffset = 0;
				region.size = src.cfg().size;

				vkCmdCopyBuffer(
					command_buffer,
					src_buffer, dst_buffer, 1, &region);
		},
		dependency(src, VK_ACCESS_TRANSFER_READ_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT),
		dependency(dst, VK_ACCESS_TRANSFER_WRITE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT));
}

vulkan_buffer_mediator::~vulkan_buffer_mediator()
{
	vmaDestroyAllocator(_vma);
}

std::unique_ptr<vulkan_buffer_mediator> vulkan_buffer_mediator::make(const config& cfg)
{
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice = cfg.physical_device;
	allocatorInfo.device = cfg.device;

	VmaAllocator vma;
	vmaCreateAllocator(&allocatorInfo, &vma);

	return std::unique_ptr < vulkan_buffer_mediator > (
		new vulkan_buffer_mediator (
			cfg,
			vma));
}

vulkan_buffer_mediator::vulkan_buffer_mediator(
	const config& cfg,
	VmaAllocator vma)
	: _cfg(cfg)
	, _vma(vma)
{
}

#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/context/vulkan_context.h>
#include <vulkan/window/vulkan_window.h>

#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>


#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#pragma warning(push)
#pragma warning(disable:4505)

//#define STBI_NO_STDIO
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#pragma warning(pop)

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <optional>
#include <set>
#include <thread>
#include <unordered_map>
#include <vulkan/window/vulkan_back_buffer.h>


const std::string MODEL_PATH = "cooked_assets/models/chalet.obj";
const std::string TEXTURE_PATH = "cooked_assets/textures/chalet.jpg";


struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 tex_coord;

	static VkVertexInputBindingDescription get_binding_description()
	{
		VkVertexInputBindingDescription binding_description = {};
		binding_description.binding = 0;
		binding_description.stride = sizeof(Vertex);
		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return binding_description;
	}

	static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions()
	{
		std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions = {};

		attribute_descriptions[0].binding = 0;
		attribute_descriptions[0].location = 0;
		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[0].offset = offsetof(Vertex, pos);

		attribute_descriptions[1].binding = 0;
		attribute_descriptions[1].location = 1;
		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attribute_descriptions[1].offset = offsetof(Vertex, color);

		attribute_descriptions[2].binding = 0;
		attribute_descriptions[2].location = 2;
		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attribute_descriptions[2].offset = offsetof(Vertex, tex_coord);

		return attribute_descriptions;
	}

	bool operator==(const Vertex& other) const
	{
		return pos == other.pos && color == other.color && tex_coord == other.tex_coord;
	}
};

namespace std
{
	template<> struct hash<Vertex>
	{
		size_t operator()(Vertex const& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.tex_coord) << 1);
		}
	};
}

struct UniformBufferObject
{
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

#include <vulkan/window/vulkan_window.h>

using namespace igpu;

class HelloTriangleApplication
{
private:
	scoped_ptr < vulkan_context > _context;
	const vulkan_window* _window = nullptr;
	
	VkInstance _instance = nullptr;
	VkPhysicalDevice _physical_device = nullptr;
	VkDevice _device = nullptr;

	scoped_ptr< vulkan_queue > _graphics_queue;
	scoped_ptr< vulkan_queue > _present_queue;

	VkDescriptorSetLayout _descriptor_set_layout;
	VkPipelineLayout _pipeline_layout;
	VkPipeline _graphics_pipeline;

	uint32_t _mip_levels;
	VkImage _texture_image;
	VkDeviceMemory _texture_image_memory;
	VkImageView _texture_image_view;
	VkSampler _texture_sampler;

	std::vector<Vertex> _vertices;
	std::vector<uint32_t> _indices;
	VkBuffer _vertex_buffer;
	VkDeviceMemory _vertex_buffer_memory;
	VkBuffer _index_buffer;
	VkDeviceMemory _index_buffer_memory;

	std::vector<VkBuffer> _uniform_buffers;
	std::vector<VkDeviceMemory> _uniform_buffers_memory;

	VkDescriptorPool _descriptor_pool;
	std::vector<VkDescriptorSet> _descriptor_sets;

	VkCommandPool _command_pool;
	
	std::vector<VkCommandBuffer> _command_buffers;
	
	const int MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkSemaphore> _image_available_semaphores;
	std::vector<VkSemaphore> _render_finished_semaphores;
	std::vector<VkFence> _in_flight_fences;
	VkSampleCountFlagBits _msaa_samples;
	size_t _swap_image_count = 0;
	size_t _current_frame = 0;

	bool _framebuffer_resized = false;
public:

	~HelloTriangleApplication()
	{
		_present_queue = nullptr;
		_graphics_queue = nullptr;
		_context = nullptr;
	}

	void framebuffer_resized()
	{
		_framebuffer_resized = true;
	}

	void init_vulkan(const scoped_ptr < vulkan_context >& context)
	{
		_context = context;
		
		_window = &_context->window();
		_instance = _context->instance();
		_physical_device = _context->physical_device();
		_device = _context->device();
		_present_queue = _context->buffer_mediator().cfg().present_queue;
		_graphics_queue = _context->buffer_mediator().cfg().graphics_queue;
		_swap_image_count = _context->back_buffer().framebuffers().size();
		_msaa_samples = _context->back_buffer().cfg().sample_count;
		create_command_pool();
		create_descriptor_set_layout();
		create_graphics_pipeline();
		create_texture_image();
		create_texture_image_view();
		create_texture_sampler();
		load_model();
		create_vertex_buffer();
		create_index_buffer();
		create_uniform_buffers();
		create_descriptor_pool();
		create_descriptor_sets();
		create_command_buffers();
		create_sync_objects();
	}

	bool main_loop()
	{
		if (_window->poll_events())
		{
			draw_frame();
			return true;
		}
		return false;
	}

	void cleanup()
	{
		on_cleanup_swap_chain();

		vkDestroySampler(_device, _texture_sampler, nullptr);
		vkDestroyImageView(_device, _texture_image_view, nullptr);

		vkDestroyImage(_device, _texture_image, nullptr);
		vkFreeMemory(_device, _texture_image_memory, nullptr);

		vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);

		vkDestroyDescriptorSetLayout(_device, _descriptor_set_layout, nullptr);

		for (VkBuffer buffer : _uniform_buffers)
		{
			vkDestroyBuffer(_device, buffer, nullptr);
		}

		for (VkDeviceMemory memory : _uniform_buffers_memory)
		{
			vkFreeMemory(_device, memory, nullptr);
		}

		vkDestroyBuffer(_device, _index_buffer, nullptr);
		vkFreeMemory(_device, _index_buffer_memory, nullptr);

		vkDestroyBuffer(_device, _vertex_buffer, nullptr);
		vkFreeMemory(_device, _vertex_buffer_memory, nullptr);

		for (auto* semaphores : {
			&_render_finished_semaphores,
			&_image_available_semaphores })
		{
			for (VkSemaphore semaphore : *semaphores)
			{
				vkDestroySemaphore(_device, semaphore, nullptr);
			}
		}
		for (VkFence fence : _in_flight_fences)
		{
			vkDestroyFence(_device, fence, nullptr);
		}
		
		vkDestroyCommandPool(_device, _command_pool, nullptr);		
	}

private:

	void on_cleanup_swap_chain()
	{

		vkDeviceWaitIdle(_device);

		vkDestroyPipeline(_device, _graphics_pipeline, nullptr);
		vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
		vkFreeCommandBuffers(_device, _command_pool, static_cast<uint32_t>(_command_buffers.size()), _command_buffers.data());
	}

	void on_recreate_swap_chain()
	{
		glm::ivec2 window_res = _window->res();

		while (window_res.x == 0 || window_res.y == 0)
		{
			_window->poll_events();
			window_res = _window->res();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}

		on_cleanup_swap_chain();
		_context->resize_back_buffer(window_res);
		create_graphics_pipeline();
		create_command_buffers();
	}

	void create_descriptor_set_layout()
	{
		VkDescriptorSetLayoutBinding ubo_layout_binding = {};
		ubo_layout_binding.binding = 0;
		ubo_layout_binding.descriptorCount = 1;
		ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo_layout_binding.pImmutableSamplers = nullptr;
		ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutBinding sampler_layout_binding = {};
		sampler_layout_binding.binding = 1;
		sampler_layout_binding.descriptorCount = 1;
		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		sampler_layout_binding.pImmutableSamplers = nullptr;
		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };
		VkDescriptorSetLayoutCreateInfo layout_info = {};
		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
		layout_info.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(_device, &layout_info, nullptr, &_descriptor_set_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	void create_graphics_pipeline()
	{
		auto vert_shader_code = read_file("cooked_assets/shaders/vert.spv");
		auto frag_shader_code = read_file("cooked_assets/shaders/frag.spv");

		VkShaderModule vert_shader_module = create_shader_module(vert_shader_code);
		VkShaderModule frag_shader_module = create_shader_module(frag_shader_code);

		VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
		vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vert_shader_stage_info.module = vert_shader_module;
		vert_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
		frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		frag_shader_stage_info.module = frag_shader_module;
		frag_shader_stage_info.pName = "main";

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };

		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto binding_description = Vertex::get_binding_description();
		auto attribute_descriptions = Vertex::get_attribute_descriptions();

		vertex_input_info.vertexBindingDescriptionCount = 1;
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
		vertex_input_info.pVertexBindingDescriptions = &binding_description;
		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();

		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		input_assembly.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_context->back_buffer().cfg().res.x;
		viewport.height = (float)_context->back_buffer().cfg().res.y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = {
			(uint32_t)_context->back_buffer().cfg().res.x,
			(uint32_t)_context->back_buffer().cfg().res.y,
		};

		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = _msaa_samples;

		VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil.depthTestEnable = VK_TRUE;
		depth_stencil.depthWriteEnable = VK_TRUE;
		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depth_stencil.depthBoundsTestEnable = VK_FALSE;
		depth_stencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		color_blend_attachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments = &color_blend_attachment;
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = 1;
		pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;

		if (vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_pipeline_layout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &input_assembly;
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = &depth_stencil;
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.layout = _pipeline_layout;
		pipeline_info.renderPass = _context->back_buffer().render_pass();;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &_graphics_pipeline) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(_device, frag_shader_module, nullptr);
		vkDestroyShaderModule(_device, vert_shader_module, nullptr);
	}
	
	void create_command_pool() {
		
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = _graphics_queue->cfg().family_index;

		if (vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics command pool!");
		}
	}

	bool has_stencil_component(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	void create_texture_image()
	{
		int tex_width, tex_height, tex_channels;
		stbi_uc * pixels = stbi_load(TEXTURE_PATH.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
		VkDeviceSize image_size = tex_width * tex_height * 4;
		_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(tex_width, tex_height)))) + 1;

		if (!pixels)
		{
			throw std::runtime_error("failed to load texture image!");
		}

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		create_buffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(_device, staging_buffer_memory, 0, image_size, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(image_size));
		vkUnmapMemory(_device, staging_buffer_memory);

		stbi_image_free(pixels);

		create_image(tex_width, tex_height, _mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _texture_image, _texture_image_memory);

		transition_image_layout(_texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _mip_levels);
		copy_buffer_to_image(staging_buffer, _texture_image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
		//transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

		vkDestroyBuffer(_device, staging_buffer, nullptr);
		vkFreeMemory(_device, staging_buffer_memory, nullptr);

		generate_mipmaps(_texture_image, VK_FORMAT_R8G8B8A8_UNORM, tex_width, tex_height, _mip_levels);
	}

	void generate_mipmaps(VkImage image, VkFormat image_format, int32_t tex_width, int32_t tex_height, uint32_t mip_levels)
	{
		// Check if image format supports linear blitting
		VkFormatProperties format_properties;
		vkGetPhysicalDeviceFormatProperties(_physical_device, image_format, &format_properties);

		if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			throw std::runtime_error("texture image format does not support linear blitting!");
		}

		VkCommandBuffer command_buffer = begin_single_time_commands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int32_t mip_width = tex_width;
		int32_t mip_height = tex_height;

		for (uint32_t i = 1; i < mip_levels; i++)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit = {};
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mip_width, mip_height, 1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.mipLevel = i - 1;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.layerCount = 1;

			vkCmdBlitImage(command_buffer,
				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &blit,
				VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			if (mip_width > 1) mip_width /= 2;
			if (mip_height > 1) mip_height /= 2;
		}

		barrier.subresourceRange.baseMipLevel = mip_levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		end_single_time_commands(command_buffer);
	}

	void create_texture_image_view()
	{
		_texture_image_view = create_image_view(_texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels);
	}

	void create_texture_sampler()
	{
		VkSamplerCreateInfo sampler_info = {};
		sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_info.magFilter = VK_FILTER_LINEAR;
		sampler_info.minFilter = VK_FILTER_LINEAR;
		sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_info.anisotropyEnable = VK_TRUE;
		sampler_info.maxAnisotropy = 16;
		sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_info.unnormalizedCoordinates = VK_FALSE;
		sampler_info.compareEnable = VK_FALSE;
		sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_info.minLod = 0;
		sampler_info.maxLod = static_cast<float>(_mip_levels);
		sampler_info.mipLodBias = 0;

		if (vkCreateSampler(_device, &sampler_info, nullptr, &_texture_sampler) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

	VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlagBits aspect_flags, uint32_t mip_levels)
	{
		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = image;
		view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
		view_info.format = format;
		view_info.subresourceRange.aspectMask = aspect_flags;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = mip_levels;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		VkImageView image_view;
		if (vkCreateImageView(_device, &view_info, nullptr, &image_view) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}

		return image_view;
	}

	void create_image(
		uint32_t width, 
		uint32_t height, 
		uint32_t mip_levels, 
		VkSampleCountFlagBits num_samples,
		VkFormat format, 
		VkImageTiling tiling,
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties, 
		VkImage& image, 
		VkDeviceMemory& image_memory)
	{
		VkImageCreateInfo image_info = {};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.imageType = VK_IMAGE_TYPE_2D;
		image_info.extent.width = width;
		image_info.extent.height = height;
		image_info.extent.depth = 1;
		image_info.mipLevels = mip_levels;
		image_info.arrayLayers = 1;
		image_info.format = format;
		image_info.tiling = tiling;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = usage;
		image_info.samples = num_samples;
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(_device, &image_info, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements mem_requirements;
		vkGetImageMemoryRequirements(_device, image, &mem_requirements);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &alloc_info, nullptr, &image_memory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(_device, image, image_memory, 0);
	}

	void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_levels)
	{
		VkCommandBuffer command_buffer = begin_single_time_commands();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;

		if (new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (has_stencil_component(format))
			{
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}
		else
		{
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		}

		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlagBits source_stage;
		VkPipelineStageFlagBits destination_stage;

		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destination_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destination_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			command_buffer,
			source_stage, destination_stage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		end_single_time_commands(command_buffer);
	}

	void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer command_buffer = begin_single_time_commands();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		end_single_time_commands(command_buffer);
	}

	void load_model()
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str()))
		{
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<Vertex, size_t> unique_vertices = {};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				auto emplace = unique_vertices.emplace(Vertex({
					{// pos
						attrib.vertices[3 * index.vertex_index + 0],
						attrib.vertices[3 * index.vertex_index + 1],
						attrib.vertices[3 * index.vertex_index + 2]
					}, { // color
						1.0f, 1.0f, 1.0f
					}, { //texcorrd
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					}
					}), unique_vertices.size());

				if (emplace.second)
				{
					_vertices.push_back(emplace.first->first);
				}

				_indices.push_back((uint32_t)emplace.first->second);
			}
		}
	}

	void create_vertex_buffer()
	{
		VkDeviceSize buffer_size = sizeof(_vertices[0]) * _vertices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(_device, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, _vertices.data(), (size_t)buffer_size);
		vkUnmapMemory(_device, staging_buffer_memory);

		create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertex_buffer, _vertex_buffer_memory);

		copy_buffer(staging_buffer, _vertex_buffer, buffer_size);

		vkDestroyBuffer(_device, staging_buffer, nullptr);
		vkFreeMemory(_device, staging_buffer_memory, nullptr);
	}

	void create_index_buffer()
	{
		VkDeviceSize buffer_size = sizeof(_indices[0]) * _indices.size();

		VkBuffer staging_buffer;
		VkDeviceMemory staging_buffer_memory;
		create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);

		void* data;
		vkMapMemory(_device, staging_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, _indices.data(), (size_t)buffer_size);
		vkUnmapMemory(_device, staging_buffer_memory);

		create_buffer(buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _index_buffer, _index_buffer_memory);

		copy_buffer(staging_buffer, _index_buffer, buffer_size);

		vkDestroyBuffer(_device, staging_buffer, nullptr);
		vkFreeMemory(_device, staging_buffer_memory, nullptr);
	}

	void create_uniform_buffers()
	{
		VkDeviceSize buffer_size = sizeof(UniformBufferObject);

		_uniform_buffers.resize(_swap_image_count);
		_uniform_buffers_memory.resize(_swap_image_count);

		for (size_t i = 0; i < _swap_image_count; i++)
		{
			create_buffer(buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniform_buffers[i], _uniform_buffers_memory[i]);
		}
	}

	void create_descriptor_pool()
	{
		std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = static_cast<uint32_t>(_swap_image_count);
		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		pool_sizes[1].descriptorCount = static_cast<uint32_t>(_swap_image_count);

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
		pool_info.pPoolSizes = pool_sizes.data();
		pool_info.maxSets = static_cast<uint32_t>(_swap_image_count);

		if (vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptor_pool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void create_descriptor_sets()
	{
		std::vector<VkDescriptorSetLayout> layouts(_swap_image_count, _descriptor_set_layout);
		VkDescriptorSetAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		alloc_info.descriptorPool = _descriptor_pool;
		alloc_info.descriptorSetCount = static_cast<uint32_t>(_swap_image_count);
		alloc_info.pSetLayouts = layouts.data();

		_descriptor_sets.resize(_swap_image_count);
		if (vkAllocateDescriptorSets(_device, &alloc_info, _descriptor_sets.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < _swap_image_count; i++)
		{
			VkDescriptorBufferInfo buffer_info = {};
			buffer_info.buffer = _uniform_buffers[i];
			buffer_info.offset = 0;
			buffer_info.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo image_info = {};
			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			image_info.imageView = _texture_image_view;
			image_info.sampler = _texture_sampler;

			std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};

			descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[0].dstSet = _descriptor_sets[i];
			descriptor_writes[0].dstBinding = 0;
			descriptor_writes[0].dstArrayElement = 0;
			descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_writes[0].descriptorCount = 1;
			descriptor_writes[0].pBufferInfo = &buffer_info;

			descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_writes[1].dstSet = _descriptor_sets[i];
			descriptor_writes[1].dstBinding = 1;
			descriptor_writes[1].dstArrayElement = 0;
			descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptor_writes[1].descriptorCount = 1;
			descriptor_writes[1].pImageInfo = &image_info;

			vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
		}
	}

	VkCommandBuffer begin_single_time_commands()
	{
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = _command_pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(_device, &alloc_info, &command_buffer);

		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &begin_info);

		return command_buffer;
	}

	void end_single_time_commands(VkCommandBuffer command_buffer)
	{
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		vkQueueSubmit(_graphics_queue->get(), 1, &submit_info, VK_NULL_HANDLE);
		vkQueueWaitIdle(_graphics_queue->get());

		vkFreeCommandBuffers(_device, _command_pool, 1, &command_buffer);
	}

	void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
	{
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = size;
		buffer_info.usage = usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements mem_requirements;
		vkGetBufferMemoryRequirements(_device, buffer, &mem_requirements);

		VkMemoryAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		alloc_info.allocationSize = mem_requirements.size;
		alloc_info.memoryTypeIndex = find_memory_type(mem_requirements.memoryTypeBits, properties);

		if (vkAllocateMemory(_device, &alloc_info, nullptr, &buffer_memory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(_device, buffer, buffer_memory, 0);
	}

	void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
	{
		VkCommandBuffer command_buffer = begin_single_time_commands();

		VkBufferCopy copy_region = {};
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer, src_buffer, dst_buffer, 1, &copy_region);

		end_single_time_commands(command_buffer);
	}

	uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(_physical_device, &mem_properties);

		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
		{
			if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void create_command_buffers()
	{
		_command_buffers.resize(_swap_image_count);

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = _command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = (uint32_t)_command_buffers.size();

		if (vkAllocateCommandBuffers(_device, &alloc_info, _command_buffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (size_t i = 0; i < _command_buffers.size(); i++)
		{
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			if (vkBeginCommandBuffer(_command_buffers[i], &begin_info) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo render_pass_info = {};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = _context->back_buffer().render_pass();
			render_pass_info.framebuffer = _context->back_buffer().framebuffers()[i];
			render_pass_info.renderArea.offset = { 0, 0 };
			render_pass_info.renderArea.extent = {
				(uint32_t)_context->back_buffer().cfg().res.x,
				(uint32_t)_context->back_buffer().cfg().res.y,
			}; 

			std::array<VkClearValue, 2> clear_values = {};
			clear_values[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
			clear_values[1].depthStencil = { 1.0f, 0 };

			render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			render_pass_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphics_pipeline);

			VkBuffer vertex_buffers[] = { _vertex_buffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(_command_buffers[i], 0, 1, vertex_buffers, offsets);

			vkCmdBindIndexBuffer(_command_buffers[i], _index_buffer, 0, VK_INDEX_TYPE_UINT32);

			vkCmdBindDescriptorSets(_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout, 0, 1, &_descriptor_sets[i], 0, nullptr);

			vkCmdDrawIndexed(_command_buffers[i], static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(_command_buffers[i]);

			if (vkEndCommandBuffer(_command_buffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void create_sync_objects()
	{
		_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
		_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(_device, &semaphore_info, nullptr, &_image_available_semaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(_device, &semaphore_info, nullptr, &_render_finished_semaphores[i]) != VK_SUCCESS ||
				vkCreateFence(_device, &fence_info, nullptr, &_in_flight_fences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}


	void update_uniform_buffer(uint32_t current_image)
	{
		static auto start_time = std::chrono::high_resolution_clock::now();

		auto current_time = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();

		UniformBufferObject ubo = {};

		auto res = _context->back_buffer().cfg().res;
#pragma warning(push)
#pragma warning(disable:4127) // constant if condition
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), res.x / (float)res.y, 0.1f, 10.0f);
#pragma warning(pop)

		ubo.proj[1][1] *= -1;

		void* data;
		vkMapMemory(_device, _uniform_buffers_memory[current_image], 0, sizeof(ubo), 0, &data);
		memcpy(data, &ubo, sizeof(ubo));
		vkUnmapMemory(_device, _uniform_buffers_memory[current_image]);
	}
	
	void draw_frame()
	{
		vkWaitForFences(_device, 1, &_in_flight_fences[_current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());

		uint32_t image_index = 0;

		switch (vkAcquireNextImageKHR(
			_device, 
			_context->back_buffer().swap_chain(),
			std::numeric_limits<uint64_t>::max(),
			_image_available_semaphores[_current_frame], 
			VK_NULL_HANDLE, 
			&image_index)) 
		{
		case VK_SUBOPTIMAL_KHR:
			LOG_WARNING("vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR");
			LOG_WARNING("vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR");
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			on_recreate_swap_chain();
			break;
		}

		vkQueueWaitIdle(_present_queue->get());

		update_uniform_buffer(image_index);

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = { _image_available_semaphores[_current_frame] };
		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_command_buffers[image_index];

		VkSemaphore signal_semaphores[] = { _render_finished_semaphores[_current_frame] };
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences(_device, 1, &_in_flight_fences[_current_frame]);

		if (vkQueueSubmit(_graphics_queue->get(), 1, &submit_info, _in_flight_fences[_current_frame]) != VK_SUCCESS)
		{
			throw std::runtime_error(EXCEPTION_CONTEXT("failed to submit draw command buffer!"));
		}

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swap_chains[] = { _context->back_buffer().swap_chain() };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swap_chains;

		present_info.pImageIndices = &image_index;

		switch (vkQueuePresentKHR(_present_queue->get(), &present_info))
		{
		case VK_SUBOPTIMAL_KHR:
			LOG_WARNING("vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR");
			[[fallthrough]];
		case VK_ERROR_OUT_OF_DATE_KHR:
			_framebuffer_resized = true;
		}

		if (_framebuffer_resized)
		{
			_framebuffer_resized = false;
			on_recreate_swap_chain();
		}

		_current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	VkShaderModule create_shader_module(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(_device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create shader module!");
		}

		return shader_module;
	}


	static std::vector<char> read_file(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t file_size = (size_t)file.tellg();
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);

		file.close();

		return buffer;
	}
};


HelloTriangleApplication hello_triangle_application;

void port_example();

void test_init_vulkan_context(const scoped_ptr < vulkan_context >& context)
{
	hello_triangle_application.init_vulkan(context);
}

bool test_loop_vulkan_context()
{	
	return hello_triangle_application.main_loop();
}

void test_cleanup_vulkan_context()
{
	hello_triangle_application.cleanup();
}
