
#include "vulkan/sync/vulkan_synchronization.h"

#include "vulkan/buffer/vulkan_buffer.h"
#include "vulkan/context/vulkan_abandon_manager.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/sync/vulkan_command_buffer.h"
#include "vulkan/sync/vulkan_fence.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/texture/vulkan_image.h"

using namespace igpu;

const vulkan_synchronization::config& vulkan_synchronization::cfg() const
{
	return _cfg;
}

size_t vulkan_synchronization::compact_queue_family_index(
	uint32_t queue_family_index ) const
{
	return _compact_queue_family_indices[queue_family_index];
}

const std::vector< scoped_ptr< vulkan_queue > >& vulkan_synchronization::
	compact_queues() const
{
	return _compact_queues;
}

VmaAllocator vulkan_synchronization::vma()
{
	return _cfg.vma;
}

vulkan_abandon_manager& vulkan_synchronization::abandon_manager() const
{
	return *_cfg.abandon_manager;
}

vulkan_synchronization::~vulkan_synchronization()
{}

std::unique_ptr< vulkan_synchronization > vulkan_synchronization::make(
	const config& cfg )
{
	std::vector< size_t > compact;
	std::vector< scoped_ptr< vulkan_queue > > compact_queues;

	for ( auto& queue : {
			  cfg.present_queue,
			  cfg.graphics_queue,
			  cfg.compute_queue,
			  cfg.transfer_queue,
		  } )
	{
		uint32_t family_index = queue->cfg().family_index;
		if ( family_index <= compact.size() )
		{
			compact.resize( family_index + 1 );
		}

		if ( 0 == compact[family_index] )
		{
			compact_queues.push_back( queue );
			compact[family_index] = compact_queues.size();
		}
	}

	for ( auto& compact_index : compact )
	{
		--compact_index;
	}

	return std::unique_ptr< vulkan_synchronization >(
		new vulkan_synchronization( cfg, compact, compact_queues ) );
}

vulkan_synchronization::vulkan_synchronization(
	const config& cfg,
	const std::vector< size_t >& compact_queue_family_indices,
	const std::vector< scoped_ptr< vulkan_queue > >& compact_queues )
	: _cfg( cfg )
	, _compact_queue_family_indices( compact_queue_family_indices )
	, _compact_queues( compact_queues )
{}

#include "vulkan/context/vulkan_context.h"
#include "vulkan/defines/vulkan_includes.h"

#include "igpu/window/window.h"

#define GLM_ENABLE_EXPERIMENTAL 1
#include "glm/glm.hpp"
#include "glm/gtx/hash.hpp"


#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader/tiny_obj_loader.h>

#pragma warning( push )
#pragma warning( disable : 4505 )

#include "vulkan/buffer/Vulkan_geometry.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/buffer/vulkan_vertex_buffer.h"
#include "vulkan/shader/vulkan_attributes_decriptor.h"
#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_primitive.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_primitives_descriptor.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"
#include "vulkan/shader/vulkan_vertex_shader.h"
#include "vulkan/sync/vulkan_barrier_manager.h"
#include "vulkan/texture/vulkan_texture2d.h"
#include "vulkan/window/vulkan_back_buffer.h"

#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <vector>

const std::string MODEL_PATH = "cooked_assets/models/chalet.obj";
const std::string TEXTURE_PATH = "cooked_assets/textures/chalet.jpg";
const std::string VERTEX_PATH = "cooked_assets/shaders/shader.vert.spv";
const std::string FRAGMENT_PATH = "cooked_assets/shaders/shader.frag.spv";

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 col;
	glm::vec2 uv0;

	bool operator==( const Vertex& other ) const
	{
		return pos == other.pos && col == other.col && uv0 == other.uv0;
	}
};

namespace std
{
	template <>
	struct hash< Vertex >
	{
		size_t operator()( Vertex const& vertex ) const
		{
			return ( ( hash< glm::vec3 >()( vertex.pos ) ^
					   ( hash< glm::vec3 >()( vertex.col ) << 1 ) ) >>
					 1 ) ^
				( hash< glm::vec2 >()( vertex.uv0 ) << 1 );
		}
	};
}

struct UniformBufferBatch
{
	alignas( 16 ) glm::mat4 view;
	alignas( 16 ) glm::mat4 proj;
};


struct UniformBufferInstance
{
	alignas( 16 ) glm::mat4 model;
};

using namespace igpu;

class HelloTriangleApplication
{
private:
	scoped_ptr< vulkan_context > _context;

	vulkan_back_buffer* vk_bb = nullptr;
	const vulkan_window* _window = nullptr;

	VkInstance _instance = nullptr;
	VkDevice _device = nullptr;

	scoped_ptr< vulkan_queue > _graphics_queue;
	scoped_ptr< vulkan_queue > _present_queue;

	VkPipeline _graphics_pipeline;

	struct new_texture_impl
	{
		HelloTriangleApplication* app;
		std::shared_ptr< texture2d > texture;
		VkSampler _texture_sampler;

		[[nodiscard]] bool create()
		{
			texture2d::config texture_cfg = {
				TEXTURE_PATH,
				sampler::filter::LINEAR,
				sampler::filter::LINEAR,
				sampler::address::WRAP,
				sampler::address::WRAP,
			};
			texture_cfg.can_auto_generate_mips = true;
			texture = app->_context->make_shared( texture_cfg );

			if ( false == app->load_buffer( TEXTURE_PATH, texture.get() ) )
			{
				return false;
			}

			return true;
		}

		VkDescriptorImageInfo descriptor_info()
		{
			auto* vulkan = (vulkan_texture2d*)texture.get();
			vulkan_image& image = vulkan->gpu_object();

			VkDescriptorImageInfo info = {};
			info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			info.imageView = image.vk_image_view();
			info.sampler = image.vk_sampler();
			return info;
		}
		void destroy()
		{
			texture = nullptr;
		}
	};

	new_texture_impl texture_impl = {
		this,
	};

	struct new_geo_impl
	{
		[[nodiscard]] bool create()
		{
			index_buffer = app->_context->make_shared( index_buffer::config{
				index_format::UNSIGNED_INT,
			} );

			vertex_buffer::attribute attr = {
				"pos",
				to_components( ( *( (Vertex*)0 ) ).pos ),
				( ( size_t ) & ( ( (Vertex*)0 )->pos ) ),
			};


			vertex_buffer = app->_context->make_shared(
				IGPU_VERT_CFG_OF( Vertex, pos, col, uv0 ) );

			geometry = app->_context->make_shared( geometry::config{
				MODEL_PATH,
				topology::TRIANGLE_LIST,
				index_buffer,
				{
					vertex_buffer,
				},
			} );

			return (bool)geometry;
		}

		[[nodiscard]] bool upload(
			const std::vector< Vertex >& vertices,
			const std::vector< uint32_t >& indices )
		{
			buffer_view< char > vertices_view(
				sizeof( vertices[0] ) * vertices.size(),
				nullptr );

			buffer_view< char > indices_view(
				sizeof( indices[0] ) * indices.size(),
				nullptr );

			index_buffer->map( &indices_view );
			vertex_buffer->map( &vertices_view );

			if ( !indices_view.data() )
			{
				LOG_CRITICAL(
					"failed to obtain mapped memory from index buffer" );
				return false;
			}

			if ( !vertices_view.data() )
			{
				LOG_CRITICAL(
					"failed to obtain mapped memory from vertex buffer" );
				return false;
			}

			memcpy(
				indices_view.data(),
				indices.data(),
				indices_view.byte_size() );
			memcpy(
				vertices_view.data(),
				vertices.data(),
				vertices_view.byte_size() );

			index_buffer->unmap();
			vertex_buffer->unmap();

			return true;
		}

		void draw( VkCommandBuffer command_buffer )
		{
			auto* vulkan = ASSERT_CAST( vulkan_geometry*, geometry.get() );
			auto& vulkan_indices = vulkan->index_buffer();

			const int MAX_BUFFERS = vertex_parameters::MAX_COUNT;
			ASSERT_CONTEXT( vulkan->cfg().vertex_buffers.size() < MAX_BUFFERS )
			VkBuffer vertex_buffers[MAX_BUFFERS];
			VkDeviceSize offsets[MAX_BUFFERS];

			for ( size_t i = 0; i < vulkan->cfg().vertex_buffers.size(); ++i )
			{
				offsets[i] = 0;
				vertex_buffers[i] =
					vulkan->vertex_buffer( i ).gpu_object().vk_buffer();
			}

			vkCmdBindIndexBuffer(
				command_buffer,
				vulkan_indices.gpu_object().vk_buffer(),
				0,
				vulkan_indices.cfg().vk.index_type );
			vkCmdBindVertexBuffers(
				command_buffer,
				0,
				1,
				vertex_buffers,
				offsets );
			vkCmdDrawIndexed(
				command_buffer,
				(uint32_t)vulkan->index_buffer().element_count(),
				1,
				0,
				0,
				0 );
		}

		void destroy()
		{
			index_buffer = nullptr;
			vertex_buffer = nullptr;
			geometry = nullptr;
		}

		HelloTriangleApplication* app;
		std::shared_ptr< geometry > geometry;
		std::shared_ptr< vertex_buffer > vertex_buffer;
		std::shared_ptr< index_buffer > index_buffer;
	};

	new_geo_impl geo_impl = {
		this,
	};

	struct batch_buffer_impl
	{
		HelloTriangleApplication* app;

		[[nodiscard]] bool create()
		{
			_compute_buffers.resize( app->_swap_image_count );

			for ( uint32_t i = 0; i < app->_swap_image_count; i++ )
			{
				_compute_buffers[i] =
					app->_context->make_shared( compute_buffer::config{} );

				if ( !_compute_buffers[i] )
				{
					return false;
				}

				update( i );
			}

			return true;
		}

		VkDescriptorBufferInfo descriptor_info( size_t swap_index )
		{
			auto vulkan =
				(vulkan_compute_buffer*)_compute_buffers[swap_index].get();
			VkBuffer buffer = vulkan->gpu_object().vk_buffer();
			VkDescriptorBufferInfo buffer_info = {};
			buffer_info.buffer = buffer;
			buffer_info.offset = 0;
			buffer_info.range = sizeof( UniformBufferBatch );
			return buffer_info;
		}

		void update( uint32_t swap_index )
		{
			auto res = app->_context->back_buffer()->res();

			auto view = buffer_view< UniformBufferBatch >( 1, nullptr );
			_compute_buffers[swap_index]->map( &view );
			UniformBufferBatch& ubo = view[0] = {};

#pragma warning( push )
#pragma warning( disable : 4127 ) // constant if condition
			ubo.view = glm::lookAt(
				glm::vec3( 2.0f, 2.0f, 2.0f ),
				glm::vec3( 0.0f, 0.0f, 0.0f ),
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
			ubo.proj = glm::perspective(
				glm::radians( 45.0f ),
				res.x / (float)res.y,
				0.1f,
				10.0f );
			ubo.proj[1][1] *= -1;
#pragma warning( pop )

			_compute_buffers[swap_index]->unmap();
		}

		void destroy()
		{
			_compute_buffers.clear();
		}

		std::vector< std::shared_ptr< compute_buffer > > _compute_buffers;
	};


	struct instance_buffer_impl : batch_buffer_impl
	{
		void update( uint32_t swap_index )
		{
			static auto start_time = std::chrono::high_resolution_clock::now();
			auto current_time = std::chrono::high_resolution_clock::now();
			float time =
				std::chrono::duration< float, std::chrono::seconds::period >(
					current_time - start_time )
					.count();

			auto view = buffer_view< UniformBufferInstance >( 1, nullptr );
			_compute_buffers[swap_index]->map( &view );
			UniformBufferInstance& ubo = view[0] = {};

#pragma warning( push )
#pragma warning( disable : 4127 ) // constant if condition
			ubo.model = glm::rotate(
				glm::mat4( 1.0f ),
				time * glm::radians( 90.0f ),
				glm::vec3( 0.0f, 0.0f, 1.0f ) );
#pragma warning( pop )

			_compute_buffers[swap_index]->unmap();
		}
	};


	batch_buffer_impl batch = {
		this,
	};


	instance_buffer_impl instance = {
		this,
	};

	struct new_program_impl
	{
		[[nodiscard]] bool create()
		{
			std::shared_ptr< vertex_shader > vertex =
				app->_context->make_shared( vertex_shader::config{} );
			std::shared_ptr< fragment_shader > fragment =
				app->_context->make_shared( fragment_shader::config{} );


			if ( !app->load_buffer( VERTEX_PATH, vertex.get() ) )
			{
				return false;
			}

			if ( !app->load_buffer( FRAGMENT_PATH, fragment.get() ) )
			{
				return false;
			}
			program = app->_context->make_shared( program::config{
				"test program",
				vertex,
				fragment,
			} );

			auto vulkan = ASSERT_CAST( vulkan_program*, program.get() );
			descriptor_set_layouts = {
				vulkan->batch_parameters().cfg().vk.layout,
				vulkan->material_parameters().cfg().vk.layout,
				vulkan->instance_parameters().cfg().vk.layout,
			};


			pipeline_layout = vulkan->pipeline_layout();
			shader_stages = {
				vulkan->cfg().vk.vertex->stage_info(),
				vulkan->cfg().vk.fragment->stage_info(),
			};

			return true;
		}

		const std::vector< VkPipelineShaderStageCreateInfo >& get_shader_stages()
		{
			return shader_stages;
		}

		void destroy()
		{
			// destroy
			program = nullptr;
		}

		HelloTriangleApplication* app;
		std::shared_ptr< program > program;
		std::array< VkDescriptorSetLayout, 3 > descriptor_set_layouts;
		VkPipelineLayout pipeline_layout;
		std::vector< VkPipelineShaderStageCreateInfo > shader_stages;
	};

	new_program_impl program_impl = {
		this,
	};

	struct new_render_states_impl
	{
		[[nodiscard]] bool create()
		{
			uint8_t color_write_mask = 0b1111;

			cull_mode c = {
				true,
				cull::BACK,
			};

			blend_mode b = {
				false,
			};

			stencil_mode s = {
				false,
			};

			depth_mode d = {
				true,
				true,
				compare::LESS,
			};

			_render_states = app->_context->make_shared( {
				color_write_mask,
				c,
				b,
				s,
				d,
			} );

			return (bool)_render_states;
		}

		void destroy()
		{
			// destroy
		}

		VkCullModeFlags cull() const
		{
			auto* vulkan =
				ASSERT_CAST( vulkan_render_states*, _render_states.get() );
			return vulkan->cfg().vk.cull;
		}

		const VkPipelineDepthStencilStateCreateInfo* depth_stencil() const
		{
			auto* vulkan =
				ASSERT_CAST( vulkan_render_states*, _render_states.get() );
			return &vulkan->cfg().vk.depth_stencil;
		}

		const VkPipelineColorBlendAttachmentState* color_blend_attachment()
			const
		{
			auto* vulkan =
				ASSERT_CAST( vulkan_render_states*, _render_states.get() );
			return &vulkan->cfg().vk.color_blend;
		}

		HelloTriangleApplication* app;
		std::shared_ptr< render_states > _render_states;
	};

	new_render_states_impl render_states_impl = {
		this,
	};

	struct new_descriptor_set_impl
	{
		HelloTriangleApplication* app = nullptr;

		struct batch_node_prims
		{
			VkDevice device;
			std::shared_ptr< igpu::primitives > primitives;
			VkDescriptorSetLayout _layout;
			VkDescriptorPool _descriptor_pool;
			VkDescriptorSet _descriptor_set;

			void setup(
				const vulkan_parameters& parameters,
				VkDevice device_,
				std::shared_ptr< igpu::primitives > primitives_ )
			{
				device = device_;
				primitives = primitives_;
				_layout = parameters.cfg().vk.layout;
				_descriptor_pool =
					create_descriptor_pool( device_, parameters, 1 );
				_descriptor_set = allocate_descriptor_set(
					device_,
					_descriptor_pool,
					_layout );

				auto* vulkan =
					ASSERT_CAST( vulkan_primitives*, primitives.get() );

				vulkan_primitives_descriptor primitives_descriptor;
				if ( primitives_descriptor.reset( parameters, *vulkan ) )
				{
					primitives_descriptor.bind_descriptor_set(
						_descriptor_set );
				}
			}

			static VkDescriptorPool create_descriptor_pool(
				VkDevice device,
				const parameters& parameters,
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

				uint32_t pool_type_count = 0;

				std::array< VkDescriptorPoolSize, 3 > pool_sizes = {};

				if ( uniform_buffers )
				{
					pool_sizes[pool_type_count++] = {
						VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
						uniform_buffers * max_sets,
					};
				}

				if ( storage_buffers )
				{
					pool_sizes[pool_type_count++] = {
						VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
						storage_buffers * max_sets,
					};
				}

				if ( samplers )
				{
					pool_sizes[pool_type_count++] = {
						VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
						samplers * max_sets,
					};
				}

				VkDescriptorPoolCreateInfo pool_info = {};
				pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
				pool_info.poolSizeCount = pool_type_count;
				pool_info.pPoolSizes = pool_sizes.data();
				pool_info.maxSets = static_cast< uint32_t >( max_sets );

				VkDescriptorPool descriptor_pool = nullptr;
				vkCreateDescriptorPool(
					device,
					&pool_info,
					nullptr,
					&descriptor_pool );
				return descriptor_pool;
			}

			static VkDescriptorSet allocate_descriptor_set(
				VkDevice device,
				VkDescriptorPool descriptor_pool,
				VkDescriptorSetLayout layout )
			{
				VkDescriptorSetAllocateInfo alloc_info = {};
				alloc_info.sType =
					VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
				alloc_info.descriptorPool = descriptor_pool;
				alloc_info.descriptorSetCount = 1;
				alloc_info.pSetLayouts = &layout;

				VkDescriptorSet set = nullptr;
				vkAllocateDescriptorSets( device, &alloc_info, &set );
				return set;
			}

			void destroy()
			{
				vkDestroyDescriptorPool( device, _descriptor_pool, nullptr );
			}
		};

		size_t _max_sets;
		std::vector< batch_node_prims > _batch_inputs;
		std::vector< batch_node_prims > _material_inputs;
		std::vector< batch_node_prims > _instance_inputs;

		void create( const vulkan_program& program, size_t max_sets )
		{
			_max_sets = max_sets;
			for ( size_t i = 0; i < max_sets; ++i )
			{
				_batch_inputs.emplace_back().setup(
					program.batch_parameters(),
					app->_device,
					app->_context->make_shared( primitives::config{ {
						{ "batch_data", app->batch._compute_buffers[i] },
					} } ) );

				_material_inputs.emplace_back().setup(
					program.material_parameters(),
					app->_device,
					app->_context->make_shared( primitives::config{ {
						{ "texSampler", app->texture_impl.texture },
					} } ) );

				_instance_inputs.emplace_back().setup(
					program.instance_parameters(),
					app->_device,
					app->_context->make_shared( primitives::config{ {
						{ "instance_data", app->instance._compute_buffers[i] },
					} } ) );
			}
		}

		void destroy()
		{
			for ( size_t i = 0; i < _max_sets; ++i )
			{
				_batch_inputs[i].destroy();
				_material_inputs[i].destroy();
				_instance_inputs[i].destroy();
			}

			_batch_inputs.clear();
			_material_inputs.clear();
			_instance_inputs.clear();
		}

		~new_descriptor_set_impl()
		{
			LOG_VERBOSE( "~new_descriptor_set_impl" );
		}
	};


	new_descriptor_set_impl descriptor_set_impl = { this };


	VkCommandPool _command_pool;

	std::vector< VkCommandBuffer > _command_buffers;

	const int MAX_FRAMES_IN_FLIGHT = 2;
	std::vector< VkSemaphore > _image_available_semaphores;
	std::vector< VkSemaphore > _render_finished_semaphores;
	std::vector< VkFence > _in_flight_fences;
	VkSampleCountFlagBits _msaa_samples;
	uint32_t _swap_image_count = 0;
	size_t _current_frame = 0;

	bool _framebuffer_resized = false;

public:
	~HelloTriangleApplication()
	{
		_present_queue = nullptr;
		_graphics_queue = nullptr;
		_context = nullptr;
	}

	bool init_vulkan( const scoped_ptr< vulkan_context >& context )
	{
		_context = context;

		vk_bb =
			ASSERT_CAST( vulkan_back_buffer*, _context->back_buffer().get() );

		_window = &_context->window();
		_device = vk_bb->cfg().vk.device;
		_present_queue = vk_bb->cfg().vk.synchronization->cfg().present_queue;
		_graphics_queue = vk_bb->cfg().vk.synchronization->cfg().graphics_queue;
		_swap_image_count = _context->cfg().vk.swap_count;
		_msaa_samples = _context->cfg().vk.sample_count;

		if ( batch.create() && instance.create() && texture_impl.create() &&
			 program_impl.create() && render_states_impl.create() &&
			 geo_impl.create() )
		{
			auto vk_program =
				ASSERT_CAST( vulkan_program*, program_impl.program.get() );
			create_command_pool();
			create_graphics_pipeline();
			descriptor_set_impl.create( *vk_program, _swap_image_count );

			if ( load_model() )
			{
				create_command_buffers();
				create_sync_objects();
				return true;
			}
		}

		LOG_CRITICAL( "failed to initialize" );
		return false;
	}

	bool main_loop()
	{
		if ( _window->poll_events() )
		{
			draw_frame();
			return true;
		}
		return false;
	}

	void cleanup()
	{
		vkDeviceWaitIdle( _device );

		on_cleanup_swap_chain();

		texture_impl.destroy();
		program_impl.destroy();

		descriptor_set_impl.destroy();

		batch.destroy();
		instance.destroy();

		geo_impl.destroy();

		for ( auto* semaphores : {
				  &_render_finished_semaphores,
				  &_image_available_semaphores,
			  } )
		{
			for ( VkSemaphore semaphore : *semaphores )
			{
				vkDestroySemaphore( _device, semaphore, nullptr );
			}
		}
		for ( VkFence fence : _in_flight_fences )
		{
			vkDestroyFence( _device, fence, nullptr );
		}

		vkDestroyCommandPool( _device, _command_pool, nullptr );
	}

private:
	void on_cleanup_swap_chain()
	{
		vkDestroyPipeline( _device, _graphics_pipeline, nullptr );
		vkFreeCommandBuffers(
			_device,
			_command_pool,
			static_cast< uint32_t >( _command_buffers.size() ),
			_command_buffers.data() );
	}

	void on_recreate_swap_chain()
	{
		glm::ivec2 window_res = _window->res();

		while ( window_res.x == 0 || window_res.y == 0 )
		{
			_window->poll_events();
			window_res = _window->res();
			std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
		}

		_context->recreate_back_buffer();

		on_cleanup_swap_chain();
		create_graphics_pipeline();
		create_command_buffers();
	}

	void create_command_pool()
	{
		VkCommandPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		pool_info.queueFamilyIndex = _graphics_queue->cfg().family_index;

		vkCreateCommandPool( _device, &pool_info, nullptr, &_command_pool );
	}

	void create_graphics_pipeline()
	{
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)_context->back_buffer()->res().x;
		viewport.height = (float)_context->back_buffer()->res().y;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.offset = {};
		scissor.extent = {
			(uint32_t)_context->back_buffer()->res().x,
			(uint32_t)_context->back_buffer()->res().y,
		};

		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType =
			VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = 1;
		viewport_state.pViewports = &viewport;
		viewport_state.scissorCount = 1;
		viewport_state.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType =
			VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = render_states_impl.cull();
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType =
			VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = _msaa_samples;

		VkPipelineColorBlendStateCreateInfo color_blending = {};
		color_blending.sType =
			VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		color_blending.logicOpEnable = VK_FALSE;
		color_blending.logicOp = VK_LOGIC_OP_COPY;
		color_blending.attachmentCount = 1;
		color_blending.pAttachments =
			render_states_impl.color_blend_attachment();
		color_blending.blendConstants[0] = 0.0f;
		color_blending.blendConstants[1] = 0.0f;
		color_blending.blendConstants[2] = 0.0f;
		color_blending.blendConstants[3] = 0.0f;


		const auto& shader_stages = program_impl.get_shader_stages();

		vulkan_program* program =
			ASSERT_CAST( vulkan_program*, program_impl.program.get() );
		vulkan_geometry* geometry =
			ASSERT_CAST( vulkan_geometry*, geo_impl.geometry.get() );

		attribute_indexer indexer;
		if ( !indexer.reset( program->vertex_parameters(), *geometry ) )
		{
			return;
		}

		vulkan_attributes_decriptor attributes_descriptor;
		attributes_descriptor.reset( indexer );

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = (uint32_t)shader_stages.size();
		pipeline_info.pStages = shader_stages.data();
		pipeline_info.pVertexInputState =
			&attributes_descriptor.vertex_input_info();
		pipeline_info.pInputAssemblyState =
			&attributes_descriptor.input_assembly();
		pipeline_info.pViewportState = &viewport_state;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisampling;
		pipeline_info.pDepthStencilState = render_states_impl.depth_stencil();
		pipeline_info.pColorBlendState = &color_blending;
		pipeline_info.layout = program_impl.pipeline_layout;
		pipeline_info.renderPass = vk_bb->render_pass();

		;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;

		vkCreateGraphicsPipelines(
			_device,
			VK_NULL_HANDLE,
			1,
			&pipeline_info,
			nullptr,
			&_graphics_pipeline );
	}

	[[nodiscard]] bool load_buffer(
		const std::string& path,
		buffer* out_buffer )
	{
		const char* err = nullptr;
		std::ifstream ifs( path.c_str(), std::ios::binary | std::ios::ate );

		if ( !out_buffer )
		{
			err = "out buffer is null";
		}
		else if ( !ifs )
		{
			err = "failed to find file";
		}
		else
		{
			buffer_view< char > view( ifs.tellg(), nullptr );
			out_buffer->map( &view );

			if ( nullptr == view.data() )
			{
				err =
					"could not obtain mapped memory from buffer to copy file "
					"into";

				ifs.close();
			}
			else
			{
				ifs.seekg( 0, std::ios::beg );
				ifs.read( view.data(), view.byte_size() );
				ifs.close();

				out_buffer->unmap();
			}
		}

		if ( err )
		{
			LOG_CRITICAL( "%s : file '%s'", err, path.c_str() );
		}


		return true;
	}

	[[nodiscard]] bool load_model()
	{
		std::vector< Vertex > vertices;
		std::vector< uint32_t > indices;

		tinyobj::attrib_t attrib;
		std::vector< tinyobj::shape_t > shapes;
		std::vector< tinyobj::material_t > materials;
		std::string warn, err;

		if ( !tinyobj::LoadObj(
				 &attrib,
				 &shapes,
				 &materials,
				 &warn,
				 &err,
				 MODEL_PATH.c_str() ) )
		{
			if ( !warn.empty() )
			{
				LOG_WARNING( "tinyobj :", warn.c_str() );
			}
			if ( !err.empty() )
			{
				LOG_CRITICAL( "tinyobj :", err.c_str() );
			}

			return false;
		}

		std::unordered_map< Vertex, size_t > unique_vertices = {};

		for ( const auto& shape : shapes )
		{
			for ( const auto& index : shape.mesh.indices )
			{
				auto emplace = unique_vertices.emplace(
					Vertex( {
						{
							// pos
							attrib.vertices[3 * index.vertex_index + 0],
							attrib.vertices[3 * index.vertex_index + 1],
							attrib.vertices[3 * index.vertex_index + 2],
						},
						{
							// color
							1.0f,
							1.0f,
							1.0f,
						},
						{
							// texcorrd
							attrib.texcoords[2 * index.texcoord_index + 0],
							1 - attrib.texcoords[2 * index.texcoord_index + 1],
						},
					} ),
					unique_vertices.size() );

				if ( emplace.second )
				{
					vertices.push_back( emplace.first->first );
				}

				indices.push_back( (uint32_t)emplace.first->second );
			}
		}

		return geo_impl.upload( vertices, indices );
	}


	void create_command_buffers()
	{
		_command_buffers.resize( _swap_image_count );

		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.commandPool = _command_pool;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandBufferCount = (uint32_t)_command_buffers.size();

		vkAllocateCommandBuffers(
			_device,
			&alloc_info,
			_command_buffers.data() );


		auto* texture =
			ASSERT_CAST( vulkan_texture2d*, texture_impl.texture.get() );

		vk_bb->cfg().vk.barrier_manager->submit_frame_job(
			_graphics_queue,
			{
				frame_job_barrier(
					&texture->gpu_object(),
					VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
					{
						decorator::READABLE,
						VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						VK_ACCESS_SHADER_READ_BIT,
					} ),
			},
			[]( auto&& ) {} );


		for ( size_t i = 0; i < _command_buffers.size(); i++ )
		{
			auto* batch_buff = ASSERT_CAST(
				vulkan_compute_buffer*,
				batch._compute_buffers[i].get() );

			auto* instance_buff = ASSERT_CAST(
				vulkan_compute_buffer*,
				instance._compute_buffers[i].get() );

			vk_bb->cfg().vk.barrier_manager->submit_frame_job(
				_graphics_queue,
				{
					frame_job_barrier(
						&batch_buff->gpu_object(),
						{
							decorator::READABLE,
							VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
							VK_ACCESS_SHADER_READ_BIT,
						} ),
					frame_job_barrier(
						&instance_buff->gpu_object(),
						{
							decorator::READABLE,
							VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
							VK_ACCESS_SHADER_READ_BIT,
						} ),
				},
				[]( auto&& ) {} );

			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			vkBeginCommandBuffer( _command_buffers[i], &begin_info );

			VkRenderPassBeginInfo render_pass_info = {};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = vk_bb->render_pass();
			render_pass_info.framebuffer = vk_bb->framebuffer( i );
			render_pass_info.renderArea.offset = {};
			render_pass_info.renderArea.extent = {
				(uint32_t)_context->back_buffer()->res().x,
				(uint32_t)_context->back_buffer()->res().y,
			};

			std::array< VkClearValue, 2 > clear_values = {};
			clear_values[0].color = {
				0.0f,
				0.0f,
				0.0f,
				1.0f,
			};
			clear_values[1].depthStencil = {
				1.0f,
				0,
			};

			render_pass_info.clearValueCount =
				static_cast< uint32_t >( clear_values.size() );
			render_pass_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(
				_command_buffers[i],
				&render_pass_info,
				VK_SUBPASS_CONTENTS_INLINE );

			vkCmdBindPipeline(
				_command_buffers[i],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				_graphics_pipeline );

			std::array< VkDescriptorSet, 3 > descriptor_sets = {
				descriptor_set_impl._batch_inputs[i]._descriptor_set,
				descriptor_set_impl._material_inputs[i]._descriptor_set,
				descriptor_set_impl._instance_inputs[i]._descriptor_set,
			};

			vkCmdBindDescriptorSets(
				_command_buffers[i],
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				program_impl.pipeline_layout,
				0,
				(uint32_t)descriptor_sets.size(),
				descriptor_sets.data(),
				0,
				nullptr );

			geo_impl.draw( _command_buffers[i] );

			vkCmdEndRenderPass( _command_buffers[i] );

			vkEndCommandBuffer( _command_buffers[i] );
		}
	}

	void create_sync_objects()
	{
		_image_available_semaphores.resize( MAX_FRAMES_IN_FLIGHT );
		_render_finished_semaphores.resize( MAX_FRAMES_IN_FLIGHT );
		_in_flight_fences.resize( MAX_FRAMES_IN_FLIGHT );

		VkSemaphoreCreateInfo semaphore_info = {};
		semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fence_info = {};
		fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for ( size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++ )
		{
			vkCreateSemaphore(
				_device,
				&semaphore_info,
				nullptr,
				&_image_available_semaphores[i] );
			vkCreateSemaphore(
				_device,
				&semaphore_info,
				nullptr,
				&_render_finished_semaphores[i] );
			vkCreateFence(
				_device,
				&fence_info,
				nullptr,
				&_in_flight_fences[i] );
		}
	}

	void draw_frame()
	{
		vkWaitForFences(
			_device,
			1,
			&_in_flight_fences[_current_frame],
			VK_TRUE,
			std::numeric_limits< uint64_t >::max() );

		uint32_t image_index = 0;

		switch ( vkAcquireNextImageKHR(
			_device,
			vk_bb->swap_chain(),
			std::numeric_limits< uint64_t >::max(),
			_image_available_semaphores[_current_frame],
			VK_NULL_HANDLE,
			&image_index ) )
		{
		case VK_SUBOPTIMAL_KHR:
			LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
			LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
			break;
		case VK_ERROR_OUT_OF_DATE_KHR:
			on_recreate_swap_chain();
			break;
		}

		vkQueueWaitIdle( _present_queue->vk_queue() );

		VkSubmitInfo submit_info = {};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore wait_semaphores[] = {
			_image_available_semaphores[_current_frame],
		};
		VkPipelineStageFlags wait_stages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		};

		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = wait_semaphores;
		submit_info.pWaitDstStageMask = wait_stages;


		// batch.update( image_index );
		// instance.update( image_index );

		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &_command_buffers[image_index];

		VkSemaphore signal_semaphores[] = {
			_render_finished_semaphores[_current_frame],
		};

		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = signal_semaphores;

		vkResetFences( _device, 1, &_in_flight_fences[_current_frame] );

		vkQueueSubmit(
			_graphics_queue->vk_queue(),
			1,
			&submit_info,
			_in_flight_fences[_current_frame] );

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		present_info.waitSemaphoreCount = 1;
		present_info.pWaitSemaphores = signal_semaphores;

		VkSwapchainKHR swap_chains[] = {
			vk_bb->swap_chain(),
		};

		present_info.swapchainCount = 1;
		present_info.pSwapchains = swap_chains;

		present_info.pImageIndices = &image_index;

		switch (
			vkQueuePresentKHR( _present_queue->vk_queue(), &present_info ) )
		{
		case VK_SUBOPTIMAL_KHR:
			LOG_WARNING( "vkAcquireNextImageKHR: VK_SUBOPTIMAL_KHR" );
			[[fallthrough]];
		case VK_ERROR_OUT_OF_DATE_KHR:
			_framebuffer_resized = true;
		}

		if ( _framebuffer_resized )
		{
			_framebuffer_resized = false;
			on_recreate_swap_chain();
		}

		_current_frame = ( _current_frame + 1 ) % MAX_FRAMES_IN_FLIGHT;
	}
};


HelloTriangleApplication hello_triangle_application;

void port_example();

bool test_init_vulkan_context( const scoped_ptr< vulkan_context >& context )
{
	return hello_triangle_application.init_vulkan( context );
}

bool test_loop_vulkan_context()
{
	return hello_triangle_application.main_loop();
}

void test_cleanup_vulkan_context()
{
	hello_triangle_application.cleanup();
}
