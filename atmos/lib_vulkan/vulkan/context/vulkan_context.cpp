
#include "vulkan/context/vulkan_context.h"

#include "vulkan/batch/vulkan_opaque_batch.h"
#include "vulkan/batch/vulkan_transparent_batch.h"
#include "vulkan/buffer/vulkan_compute_buffer.h"
#include "vulkan/buffer/vulkan_geometry.h"
#include "vulkan/buffer/vulkan_index_buffer.h"
#include "vulkan/buffer/vulkan_vertex_buffer.h"
#include "vulkan/defines/vulkan_includes.h"
#include "vulkan/shader/vulkan_fragment_shader.h"
#include "vulkan/shader/vulkan_graphics_pipeline.h"
#include "vulkan/shader/vulkan_pipeline_cache.h"
#include "vulkan/shader/vulkan_primitives.h"
#include "vulkan/shader/vulkan_program.h"
#include "vulkan/shader/vulkan_render_states.h"
#include "vulkan/shader/vulkan_vertex_shader.h"
#include "vulkan/sync/vulkan_queue.h"
#include "vulkan/sync/vulkan_synchronization.h"
#include "vulkan/texture/vulkan_depth_buffer.h"
#include "vulkan/texture/vulkan_depth_texture2d.h"
#include "vulkan/texture/vulkan_draw_target.h"
#include "vulkan/texture/vulkan_render_buffer.h"
#include "vulkan/texture/vulkan_render_texture2d.h"
#include "vulkan/texture/vulkan_texture2d.h"
#include "vulkan/window/vulkan_back_buffer.h"
#include "vulkan/window/vulkan_window.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <set>
#include <vector>

using namespace igpu;

namespace
{
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance instance,
		const VkDebugUtilsMessengerCreateInfoEXT* p_create_info,
		const VkAllocationCallbacks* p_allocator,
		VkDebugUtilsMessengerEXT* p_debug_messenger )
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkCreateDebugUtilsMessengerEXT" );
		if ( func != nullptr )
		{
			return func(
				instance,
				p_create_info,
				p_allocator,
				p_debug_messenger );
		}

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkResult DestroyDebugUtilsMessengerEXT(
		VkInstance instance,
		VkDebugUtilsMessengerEXT debug_messenger,
		const VkAllocationCallbacks* p_allocator )
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
			instance,
			"vkDestroyDebugUtilsMessengerEXT" );
		if ( func != nullptr )
		{
			func( instance, debug_messenger, p_allocator );
			return VK_SUCCESS;
		}

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkInstance create_instance( const context::config& cfg )
	{
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = cfg.name.data();
		app_info.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
		app_info.pEngineName = "No Engine";
		app_info.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
		app_info.apiVersion = VK_API_VERSION_1_0;


		std::vector< const char* > extensions =
			vulkan_window::required_extensions();
		std::vector< const char* > layers;

		if ( cfg.enable_validation )
		{
			extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
			layers.push_back( "VK_LAYER_LUNARG_standard_validation" );
		}

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;
		create_info.enabledExtensionCount = (uint32_t)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();
		create_info.enabledLayerCount = (uint32_t)layers.size();
		create_info.ppEnabledLayerNames = layers.data();

		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties( &layer_count, nullptr );

		std::vector< VkLayerProperties > available_layers( layer_count );
		vkEnumerateInstanceLayerProperties(
			&layer_count,
			available_layers.data() );

		VkInstance instance = nullptr;

		for ( uint32_t i = 0; i < create_info.enabledLayerCount; ++i )
		{
			const char* layer_name = create_info.ppEnabledLayerNames[i];
			bool found = false;
			for ( const VkLayerProperties& properties : available_layers )
			{
				if ( strcmp( layer_name, properties.layerName ) == 0 )
				{
					found = true;
					break;
				}
			}

			if ( !found )
			{
				LOG_CRITICAL( "%s requested, but not available", layer_name );
			}
		}

		vkCreateInstance( &create_info, nullptr, &instance );
		return instance;
	}

#if ATMOS_DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
		void* /* p_user_data */ )
	{
		// VkDebugUtilsMessengerEXT envokes our callback twice for each
		// message?? wat
		static struct
		{
			VkDebugUtilsMessageSeverityFlagBitsEXT severity;
			VkDebugUtilsMessageTypeFlagsEXT type;
			std::string message;
		} prev = {};

		if ( prev.severity == message_severity && prev.type == message_type &&
			 prev.message == p_callback_data->pMessage )
		{
			return VK_FALSE;
		}

		prev.severity = message_severity;
		prev.type = message_type;
		prev.message = p_callback_data->pMessage;


		logging::severity severity = logging::severity::VERBOSE;

		if ( message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT )
		{
			severity = logging::severity::CRITICAL;
		}
		else if (
			message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT )
		{
			severity = logging::severity::WARNING;
		}
		else if (
			message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT )
		{
			severity = logging::severity::DEBUG;
		}

		std::string message;
		if ( message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT )
		{
			message = "< general message >\n";
		}
		else if (
			message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT )
		{
			message = "< validation message >\n";
		}
		else if (
			message_type & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT )
		{
			message = "< performance message >\n";
		}

		message += p_callback_data->pMessage;

		igpu::debug::set_callback_info( {
			severity,
			message.c_str(),
		} );

		return VK_FALSE;
	}

	VkDebugUtilsMessengerEXT create_debug_messenger(
		const context::config& cfg,
		VkInstance instance )
	{
		VkDebugUtilsMessengerEXT debug_messenger = nullptr;

		if ( cfg.enable_validation )
		{
			VkDebugUtilsMessengerCreateInfoEXT create_info = {};
			create_info.sType =
				VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			create_info.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			create_info.pfnUserCallback = debug_callback;
			create_info.messageSeverity = 0
				//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			if ( CreateDebugUtilsMessengerEXT(
					 instance,
					 &create_info,
					 nullptr,
					 &debug_messenger ) != VK_SUCCESS )
			{
				LOG_CRITICAL( "failed to set up debug messenger!" );
			}
		}

		return debug_messenger;
	}
#endif // ATMOS_DEBUG

	struct queue_families
	{
		enum fitness_type
		{
			ALL,  // supports all family types
			SOME, // supports some family types
			ONE,  // suports one family type
		};

		struct family
		{
			const char* const name;
			const VkQueueFlagBits flags = (VkQueueFlagBits)0;
			const bool require_present = 0;
			std::optional< uint32_t > family_index;
			fitness_type fitness = ALL;
		};

		family present{
			"present",
			VK_QUEUE_GRAPHICS_BIT,
			true,
		};
		family graphics{
			"graphics",
			VK_QUEUE_GRAPHICS_BIT,
		};
		family compute{
			"compute",
			VK_QUEUE_COMPUTE_BIT,
		};
		family transfer{
			"transfer",
			VK_QUEUE_TRANSFER_BIT,
		};
	};

	// determine best queue families to use
	// prescendence of fitness type for each family is ONE, then SOME, then ALL
	bool assign_queue_families(
		queue_families* families,
		VkPhysicalDevice physical_device,
		VkSurfaceKHR surface_khr )
	{
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device,
			&queue_family_count,
			nullptr );

		std::vector< VkQueueFamilyProperties > properties( queue_family_count );
		vkGetPhysicalDeviceQueueFamilyProperties(
			physical_device,
			&queue_family_count,
			properties.data() );
		VkFlags all_flags = 0;
		for ( auto* family : {
				  &families->present,
				  &families->graphics,
				  &families->compute,
				  &families->transfer,
			  } )
		{
			all_flags |= family->flags;
		}

		for ( auto* family : {
				  &families->present,
				  &families->graphics,
				  &families->compute,
				  &families->transfer,
			  } )
		{
			family->family_index.reset();
			family->fitness = queue_families::ALL;

			for ( uint32_t i = 0; i < properties.size(); ++i )
			{
				if ( family->require_present )
				{
					VkBool32 present_support = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(
						physical_device,
						i,
						surface_khr,
						&present_support );

					if ( !present_support )
					{
						continue;
					}
				}

				VkFlags flags = all_flags & properties[i].queueFlags;
				switch ( family->fitness )
				{
				case queue_families::ALL:
					if ( flags == all_flags )
					{
						family->fitness = queue_families::ALL;
						family->family_index = i;
					}
					[[fallthrough]];
				case queue_families::SOME:
					if ( flags != all_flags &&
						 flags != (VkFlags)family->flags &&
						 flags & (VkFlags)family->flags )
					{
						family->fitness = queue_families::SOME;
						family->family_index = i;
					}
					[[fallthrough]];
				case queue_families::ONE:
					if ( flags == (VkFlags)family->flags )
					{
						family->fitness = queue_families::ONE;
						family->family_index = i;
					}
				}
			}

			if ( !family->family_index.has_value() )
			{
				return false;
			}
		}

		return true;
	}

	VkPhysicalDevice pick_physical_device(
		queue_families* families,
		VkInstance instance,
		VkSurfaceKHR surface_khr )
	{
		uint32_t physical_device_count = 0;
		std::vector< VkPhysicalDevice > physical_devices;

		vkEnumeratePhysicalDevices( instance, &physical_device_count, nullptr );
		physical_devices.resize( physical_device_count );
		vkEnumeratePhysicalDevices(
			instance,
			&physical_device_count,
			physical_devices.data() );

		for ( const auto& physical_device : physical_devices )
		{
			if ( assign_queue_families(
					 families,
					 physical_device,
					 surface_khr ) )
			{
				return physical_device;
			}
		}

		if ( physical_device_count == 0 )
		{
			LOG_CRITICAL( "failed to find GPUs with Vulkan support" );
		}

		LOG_CRITICAL(
			"failed to find GPU with queue families "
			"present/graphics/compute/transfer" );

		return nullptr;
	}

	VkDevice create_device(
		VkPhysicalDevice physical_device,
		const queue_families& families )
	{
		const float queue_priority = 1.0;
		std::vector< VkDeviceQueueCreateInfo > queue_create_infos;
		for ( auto queue_family : std::set< uint32_t >( {

				  families.present.family_index.value(),
				  families.graphics.family_index.value(),
				  families.compute.family_index.value(),
				  families.transfer.family_index.value(),
			  } ) )
		{
			VkDeviceQueueCreateInfo queue_create_info = {};

			queue_create_info.sType =
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;

			queue_create_infos.push_back( queue_create_info );
		}

		const std::vector< const char* > device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
		};


		VkPhysicalDeviceFeatures device_features = {};
		device_features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount =
			static_cast< uint32_t >( queue_create_infos.size() );
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.pEnabledFeatures = &device_features;
		create_info.enabledExtensionCount =
			static_cast< uint32_t >( device_extensions.size() );
		create_info.ppEnabledExtensionNames = device_extensions.data();

		VkDevice device = nullptr;
		vkCreateDevice( physical_device, &create_info, nullptr, &device );
		return device;
	}

	VkSampleCountFlagBits get_max_usable_sample_count(
		const vulkan_context::config& cfg )
	{
		const auto& properties = cfg.vk.physical_device_properties;
		VkSampleCountFlags counts =
			properties.limits.framebufferColorSampleCounts <
				properties.limits.framebufferDepthSampleCounts
			? properties.limits.framebufferColorSampleCounts
			: properties.limits.framebufferDepthSampleCounts;

		if ( counts & VK_SAMPLE_COUNT_64_BIT )
		{
			return VK_SAMPLE_COUNT_64_BIT;
		}
		if ( counts & VK_SAMPLE_COUNT_32_BIT )
		{
			return VK_SAMPLE_COUNT_32_BIT;
		}
		if ( counts & VK_SAMPLE_COUNT_16_BIT )
		{
			return VK_SAMPLE_COUNT_16_BIT;
		}
		if ( counts & VK_SAMPLE_COUNT_8_BIT )
		{
			return VK_SAMPLE_COUNT_8_BIT;
		}
		if ( counts & VK_SAMPLE_COUNT_4_BIT )
		{
			return VK_SAMPLE_COUNT_4_BIT;
		}
		if ( counts & VK_SAMPLE_COUNT_2_BIT )
		{
			return VK_SAMPLE_COUNT_2_BIT;
		}

		return VK_SAMPLE_COUNT_1_BIT;
	}

	glm::ivec2 choose_screen_res(
		const glm::ivec2 screen_res,
		const VkSurfaceCapabilitiesKHR& capabilities )
	{
		if ( capabilities.currentExtent.width !=
			 std::numeric_limits< uint32_t >::max() )
		{
			return {
				capabilities.currentExtent.width,
				capabilities.currentExtent.height,
			};
		}
		return {
			std::max(
				capabilities.minImageExtent.width,
				std::min(
					capabilities.maxImageExtent.width,
					(uint32_t)screen_res.x ) ),
			std::max(
				capabilities.minImageExtent.height,
				std::min(
					capabilities.maxImageExtent.height,
					(uint32_t)screen_res.y ) ),
		};
	}

	std::unique_ptr< vulkan_back_buffer > create_back_buffer(
		const vulkan_context::config& cfg,
		VkPhysicalDevice physical_device,
		VkSurfaceKHR surface,
		VkDevice device,
		const glm::ivec2& res,
		uint32_t present_queue_family,
		uint32_t graphics_queue_family )
	{
		VkSurfaceCapabilitiesKHR surface_caps = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
			physical_device,
			surface,
			&surface_caps );

		sampler samp = {
			sampler::filter::LINEAR,
			sampler::filter::LINEAR,
			sampler::address::MIRROR,
			sampler::address::MIRROR,
		};

		return vulkan_back_buffer::make( {
			cfg.color_format,
			cfg.depth_format,
			samp,
			choose_screen_res( res, surface_caps ),
			physical_device,
			surface,
			device,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			present_queue_family,
			graphics_queue_family,
			get_max_usable_sample_count( cfg ),
		} );
	}
}

std::unique_ptr< vulkan_context > vulkan_context::make(
	const context::config& base_cfg,
	const glm::ivec2& screen_res )
{
	VkInstance instance = create_instance( base_cfg );
	VkDebugUtilsMessengerEXT debug_messenger = nullptr;

#if ATMOS_DEBUG
	debug_messenger = create_debug_messenger( base_cfg, instance );
#endif

	vulkan_window::config window_cfg = {};
	window_cfg.name = base_cfg.name;
	window_cfg.vk.instance = instance;
	auto window = vulkan_window::make( window_cfg, screen_res );
	if ( !window )
	{
		return nullptr;
	}

	queue_families families;
	VkPhysicalDevice physical_device =
		pick_physical_device( &families, instance, window->surface() );
	if ( !physical_device )
	{
		return nullptr;
	}

	VkDevice device = create_device( physical_device, families );
	if ( !device )
	{
		return nullptr;
	}

	vulkan_context::config cfg = {
		base_cfg,
	};
	vkGetPhysicalDeviceProperties(
		physical_device,
		&cfg.vk.physical_device_properties );

	std::shared_ptr< vulkan_queue >
		present_queue = vulkan_queue::make(
			{ device, families.present.family_index.value(), 0 } ),
		graphics_queue = vulkan_queue::make(
			{ device, families.graphics.family_index.value(), 0 } ),
		compute_queue = vulkan_queue::make(
			{ device, families.compute.family_index.value(), 0 } ),
		transfer_queue = vulkan_queue::make(
			{ device, families.transfer.family_index.value(), 0 } );

	auto pipeline_cache = vulkan_pipeline_cache::make( {
		device,
	} );

	vulkan_synchronization::config synchronization_cfg = {};
	synchronization_cfg.physical_device = physical_device;
	synchronization_cfg.device = device;
	synchronization_cfg.present_queue = present_queue;
	synchronization_cfg.graphics_queue = graphics_queue;
	synchronization_cfg.compute_queue = compute_queue;
	synchronization_cfg.transfer_queue = transfer_queue;
	auto synchronization = vulkan_synchronization::make( synchronization_cfg );
	if ( !synchronization )
	{
		return nullptr;
	}

	auto back_buffer = create_back_buffer(
		cfg,
		physical_device,
		window->surface(),
		device,
		screen_res,
		present_queue->cfg().family_index,
		graphics_queue->cfg().family_index );
	if ( !back_buffer )
	{
		return nullptr;
	}

	return std::unique_ptr< vulkan_context >( new vulkan_context(
		cfg,
		instance,
		debug_messenger,
		physical_device,
		device,
		present_queue,
		graphics_queue,
		compute_queue,
		transfer_queue,
		std::move( pipeline_cache ),
		std::move( synchronization ),
		std::move( window ),
		std::move( back_buffer ) ) );
}

const vulkan_context::config& vulkan_context::cfg() const
{
	return _cfg;
}

std::unique_ptr< graphics_pipeline > vulkan_context::make(
	const graphics_pipeline::config& base_cfg )
{
	auto draw_target =
		std::dynamic_pointer_cast< vulkan_draw_target, igpu::draw_target >(
			base_cfg.draw_target );

	auto program = std::dynamic_pointer_cast< vulkan_program, igpu::program >(
		base_cfg.program );

	auto render_states =
		std::dynamic_pointer_cast< vulkan_render_states, igpu::render_states >(
			base_cfg.render_states );

	return vulkan_graphics_pipeline::make(
		{
			base_cfg,
			_state.device,
			_pipeline_cache,
			draw_target,
			program,
			render_states,
		},
		_back_buffer->cfg().res );
}

std::unique_ptr< draw_target > vulkan_context::make(
	const draw_target::config& base_cfg )
{
	auto color =
		std::dynamic_pointer_cast< vulkan_render_buffer, igpu::render_buffer >(
			base_cfg.color );

	auto depth =
		std::dynamic_pointer_cast< vulkan_depth_buffer, igpu::depth_buffer >(
			base_cfg.depth );

	return vulkan_draw_target::make( {
		base_cfg,
		_state.device,
		color,
		depth,
	} );
}

std::unique_ptr< render_buffer > vulkan_context::make(
	const render_buffer::config& base_cfg )
{
	return vulkan_render_buffer::make( {
		base_cfg,
		_state.physical_device,
		_state.device,
		to_vulkan_format( base_cfg.format ),
		_back_buffer->cfg().vk.sample_count,
		VK_SHARING_MODE_EXCLUSIVE,
	} );
}

std::unique_ptr< render_texture2d > vulkan_context::make(
	const render_texture2d::config& base_cfg )
{
	return vulkan_render_texture2d::make( {
		base_cfg,
		_state.physical_device,
		_state.device,
		to_vulkan_format( base_cfg.format ),
		_back_buffer->cfg().vk.sample_count,
		VK_SHARING_MODE_EXCLUSIVE,
	} );
}

std::unique_ptr< depth_buffer > vulkan_context::make(
	const depth_buffer::config& base_cfg )
{
	return vulkan_depth_buffer::make( {
		base_cfg,
		_state.physical_device,
		_state.device,
		to_vulkan_format( base_cfg.format ),
		_back_buffer->cfg().vk.sample_count,
		VK_SHARING_MODE_EXCLUSIVE,
	} );
}

std::unique_ptr< depth_texture2d > vulkan_context::make(
	const depth_texture2d::config& base_cfg )
{
	return vulkan_depth_texture2d::make( {
		base_cfg,
		_state.physical_device,
		_state.device,
		to_vulkan_format( base_cfg.format ),
		_back_buffer->cfg().vk.sample_count,
		VK_SHARING_MODE_EXCLUSIVE,
	} );
}

std::unique_ptr< program > vulkan_context::make(
	const program::config& base_cfg )
{
	auto vertex =
		std::dynamic_pointer_cast< vulkan_vertex_shader, vertex_shader >(
			base_cfg.vertex );

	auto fragment =
		std::dynamic_pointer_cast< vulkan_fragment_shader, fragment_shader >(
			base_cfg.fragment );

	return vulkan_program::make( {
		base_cfg,
		_state.device,
		vertex,
		fragment,
	} );
}

std::unique_ptr< vertex_shader > vulkan_context::make(
	const vertex_shader::config& )
{
	return vulkan_vertex_shader::make( {
		this->_state.device,
		VK_SHADER_STAGE_VERTEX_BIT,
	} );
}

std::unique_ptr< fragment_shader > vulkan_context::make(
	const fragment_shader::config& )
{
	return vulkan_fragment_shader::make( {
		this->_state.device,
		VK_SHADER_STAGE_FRAGMENT_BIT,
	} );
}

std::unique_ptr< render_states > vulkan_context::make(
	const render_states::config& base_cfg )
{
	vulkan_render_states::config cfg = {
		base_cfg,
		to_vulkan_render_states( base_cfg ),
	};

	return std::unique_ptr< render_states >( new vulkan_render_states( cfg ) );
}

std::unique_ptr< geometry > vulkan_context::make( const geometry::config& cfg )
{
	return vulkan_geometry::make( {
		cfg,
		to_vulkan_topology( cfg.topology ),
	} );
}

std::unique_ptr< vertex_buffer > vulkan_context::make(
	const vertex_buffer::config& cfg )
{
	return vulkan_vertex_buffer::make(
		{
			cfg,
			_state.device,
			&_cfg.vk.physical_device_properties,
		},
		_synchronization );
}

std::unique_ptr< index_buffer > vulkan_context::make(
	const index_buffer::config& base_cfg )
{
	return vulkan_index_buffer::make(
		{
			base_cfg,
			_state.device,
			&_cfg.vk.physical_device_properties,
			to_vulkan_format( base_cfg.format ),
		},
		_synchronization );
}

std::unique_ptr< compute_buffer > vulkan_context::make(
	const compute_buffer::config& cfg )
{
	return vulkan_compute_buffer::make(
		{
			cfg,
			_state.device,
			&_cfg.vk.physical_device_properties,
		},
		_synchronization );
}

std::unique_ptr< texture2d > vulkan_context::make(
	const texture2d::config& cfg )
{
	return vulkan_texture2d::make(
		{
			cfg,
			&_cfg.vk.physical_device_properties,
			_state.physical_device,
			_state.device,
		},
		_synchronization );
}

std::unique_ptr< primitives > vulkan_context::make(
	const primitives::config& cfg )
{
	return vulkan_primitives::make( cfg );
}

std::unique_ptr< opaque_batch > vulkan_context::make(
	const opaque_batch::config& base_cfg )
{
	auto draw_target =
		std::dynamic_pointer_cast< vulkan_draw_target, igpu::draw_target >(
			base_cfg.draw_target );

	return vulkan_opaque_batch::make( {
		base_cfg,
		_state.device,
		draw_target,
		_back_buffer->framebuffers().size(),
		_pipeline_cache,
	} );
}

std::unique_ptr< transparent_batch > vulkan_context::make(
	const transparent_batch::config& base_cfg )
{
	auto draw_target =
		std::dynamic_pointer_cast< vulkan_draw_target, igpu::draw_target >(
			base_cfg.draw_target );

	return vulkan_transparent_batch::make( {
		base_cfg,
		_state.device,
		draw_target,
		_back_buffer->framebuffers().size(),
		_pipeline_cache,
	} );
}

const vulkan_window& vulkan_context::window() const
{
	return *_window;
}

const vulkan_back_buffer& vulkan_context::back_buffer() const
{
	return *_back_buffer;
}

vulkan_context::vulkan_context(
	const config& cfg,
	VkInstance instance,
	VkDebugUtilsMessengerEXT debug_messenger,
	VkPhysicalDevice physical_device,
	VkDevice device,
	const std::shared_ptr< vulkan_queue >& present_queue,
	const std::shared_ptr< vulkan_queue >& graphics_queue,
	const std::shared_ptr< vulkan_queue >& compute_queue,
	const std::shared_ptr< vulkan_queue >& transfer_queue,
	const std::shared_ptr< vulkan_pipeline_cache >& pipeline_cache,
	const std::shared_ptr< vulkan_synchronization >& synchronization,
	std::unique_ptr< vulkan_window > window,
	std::unique_ptr< vulkan_back_buffer > back_buffer )
	: _cfg( cfg )
	, _state{ instance, debug_messenger, physical_device, device, }
	, _present_queue( present_queue )
	, _graphics_queue( graphics_queue )
	, _compute_queue( compute_queue )
	, _transfer_queue( transfer_queue )
	, _pipeline_cache(pipeline_cache)
	, _synchronization( synchronization )
	, _window( std::move( window ) )
	, _back_buffer( std::move( back_buffer ) )
#if ATMOS_PERFORMANCE_TRACKING
	, _renderstate_switch_metric(
		  perf::category::SWITCH_RENDER_STATES,
		  "Renderstates" )
	, _draw_target_clears_metric(
		  perf::category::CLEAR_DRAW_TARGET,
		  "Draw Target Clears" )
	, _draw_target_switch_metric(
		  perf::category::SWITCH_DRAW_TARGET,
		  "Draw Target Changes" )
	, _shader_switch_metric( perf::category::SWITCH_SHADERS, "Shaders" )
	, _geometry_switch_metric(
		  perf::category::SWITCH_GEOMETRY,
		  "geometry Count" )
	, _draw_call_metric( perf::category::DRAW_CALL_COUNT, "Draw Calls" )
	, _polycount_metric( perf::category::POLY_COUNT, "Polycount" )
#endif
{}

vulkan_context::auto_destroy::~auto_destroy()
{
	vkDestroyDevice( device, nullptr );

	if ( debug_messenger )
	{
		DestroyDebugUtilsMessengerEXT( instance, debug_messenger, nullptr );
	}

	vkDestroyInstance( instance, nullptr );
}

vulkan_context::~vulkan_context()
{}

void vulkan_context::resize_back_buffer( const glm::ivec2& screen_res )
{
	_back_buffer = nullptr;
	_back_buffer = ::create_back_buffer(
		_cfg,
		_state.physical_device,
		_window->surface(),
		_state.device,
		screen_res,
		_present_queue->cfg().family_index,
		_graphics_queue->cfg().family_index );
}
