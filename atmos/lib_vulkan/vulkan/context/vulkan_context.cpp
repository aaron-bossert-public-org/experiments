
#include <vulkan/context/vulkan_context.h>

// Vulkan implementation includes - begin
#include <vulkan/batch/vulkan_opaque_batch.h>
#include <vulkan/batch/vulkan_transparent_batch.h>
#include <vulkan/buffer/vulkan_geometry.h>
#include <vulkan/buffer/vulkan_buffer_mediator.h>
#include <vulkan/buffer/vulkan_compute_buffer.h>
#include <vulkan/buffer/vulkan_index_buffer.h>
#include <vulkan/buffer/vulkan_queue.h>
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/material/vulkan_fragment_shader.h>
#include <vulkan/material/vulkan_program.h>
#include <vulkan/material/vulkan_vertex_shader.h>
#include <vulkan/texture/vulkan_texture2d.h>
#include <vulkan/window/vulkan_back_buffer.h>
#include <vulkan/window/vulkan_window.h>
// Vulkan implementation includes - end

#include <optional>
#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace igpu;

namespace
{
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, p_create_info, p_allocator, p_debug_messenger);
		}
	
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkResult DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debug_messenger, p_allocator);
			return VK_SUCCESS;
		}

		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	VkInstance create_instance(
		const vulkan_context::config& cfg)
	{
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pApplicationName = cfg.name.data();
		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.pEngineName = "No Engine";
		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		app_info.apiVersion = VK_API_VERSION_1_0;


		std::vector<const char*> extensions = vulkan_window::required_extensions();
		std::vector<const char*> layers;

		if (cfg.enable_validation)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			layers.push_back("VK_LAYER_LUNARG_standard_validation");
		}

		VkInstanceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info;
		create_info.enabledExtensionCount = (uint32_t)extensions.size();
		create_info.ppEnabledExtensionNames = extensions.data();
		create_info.enabledLayerCount = (uint32_t)layers.size();
		create_info.ppEnabledLayerNames = layers.data();

		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		VkInstance instance = nullptr;

		for (uint32_t i = 0; i < create_info.enabledLayerCount; ++i)
		{
			const char* layer_name = create_info.ppEnabledLayerNames[i];
			bool found = false;
			for (const VkLayerProperties& properties : available_layers)
			{
				if (strcmp(layer_name, properties.layerName) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				LOG_CRITICAL("%s requested, but not available", layer_name);
			}
		}

		vkCreateInstance(&create_info, nullptr, &instance);
		return instance;
	}
	
#if ATMOS_DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT /* message_type */, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* /* p_user_data */)
	{
		logging::severity severity = logging::severity::VERBOSE;

		if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			severity = logging::severity::CRITICAL;
		}
		else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		{
			severity = logging::severity::WARNING;
		}
		else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
		{
			severity = logging::severity::DEBUG;
		}
		else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
		{
			severity = logging::severity::VERBOSE;
		}

		igpu::debug::set_callback_info({
			severity,
			p_callback_data->pMessage });

		return VK_FALSE;
	}

	VkDebugUtilsMessengerEXT create_debug_messenger(
		const vulkan_context::config& cfg,
		VkInstance instance)
	{
		VkDebugUtilsMessengerEXT debug_messenger = nullptr;
		
		if (cfg.enable_validation)
		{
			VkDebugUtilsMessengerCreateInfoEXT create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			create_info.pfnUserCallback = debug_callback;
			create_info.messageSeverity = 0
				//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				//| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT
				;
			
			if (CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to set up debug messenger!");
			}
		}
		
		return debug_messenger;
	}
#endif //ATMOS_DEBUG

	struct queue_families
	{
		enum fitness_type
		{
			ALL, // supports all family types
			SOME, // supports some family types
			ONE, // suports one family type
		};

		struct
		{
			const char* const name;
			const VkQueueFlagBits flags = (VkQueueFlagBits)0;
			const bool require_present = 0;
			std::optional < uint32_t > family_index;
			fitness_type fitness = ALL;

		}	present{ "present", VK_QUEUE_GRAPHICS_BIT, true },
			graphics{ "graphics", VK_QUEUE_GRAPHICS_BIT },
			compute{ "compute", VK_QUEUE_COMPUTE_BIT },
			transfer{ "transfer", VK_QUEUE_TRANSFER_BIT };
	};

	// determine best queue families to use
	// prescendence of fitness type for each family is ONE, then SOME, then ALL
	bool assign_queue_families(queue_families* families, VkPhysicalDevice physical_device, VkSurfaceKHR surface_khr)
	{
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

		std::vector<VkQueueFamilyProperties> properties(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, properties.data());
		VkFlags all_flags = 0;
		for (auto* family : { &families->present, &families->graphics, &families->compute, &families->transfer })
		{
			all_flags |= family->flags;
		}

		for (auto* family : { &families->present, &families->graphics, &families->compute, &families->transfer })
		{
			family->family_index.reset();
			family->fitness = queue_families::ALL;

			for (uint32_t i = 0; i < properties.size(); ++i)
			{
				if (family->require_present)
				{
					VkBool32 present_support = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface_khr, &present_support);

					if (!present_support)
					{
						continue;
					}
				}

				VkFlags flags = all_flags & properties[i].queueFlags;
				switch (family->fitness)
				{
				case queue_families::ALL:
					if (flags == all_flags)
					{
						family->fitness = queue_families::ALL;
						family->family_index = i;
					}
					[[fallthrough]];
				case queue_families::SOME:
					if (flags != all_flags && flags != (VkFlags)family->flags && flags & (VkFlags)family->flags)
					{
						family->fitness = queue_families::SOME;
						family->family_index = i;
					}
					[[fallthrough]];
				case queue_families::ONE:
					if (flags == (VkFlags)family->flags)
					{
						family->fitness = queue_families::ONE;
						family->family_index = i;
					}
				}
			}

			if (!family->family_index.has_value())
			{
				return false;
			}
		}

		return true;
	}

	VkPhysicalDevice pick_physical_device(queue_families* families, VkInstance instance, VkSurfaceKHR surface_khr)
	{
		uint32_t physical_device_count = 0;
		std::vector<VkPhysicalDevice> physical_devices;

		vkEnumeratePhysicalDevices(instance, &physical_device_count, nullptr);
		physical_devices.resize(physical_device_count);
		vkEnumeratePhysicalDevices(instance, &physical_device_count, physical_devices.data());

		for (const auto& physical_device : physical_devices)
		{
			if (assign_queue_families(families, physical_device, surface_khr))
			{
				return physical_device;
			}
		}

		if (physical_device_count == 0)
		{
			throw std::runtime_error(EXCEPTION_CONTEXT("failed to find GPUs with Vulkan support"));
		}

		throw std::runtime_error(EXCEPTION_CONTEXT("failed to find GPU with queue families present/graphcis/compute/transfer"));
	}

	VkDevice create_device(VkPhysicalDevice physical_device, const queue_families& families)
	{
		const float queue_priority = 1.0;
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		for (auto queue_family : std::set<uint32_t>({

			families.present.family_index.value(),
			families.graphics.family_index.value(),
			families.compute.family_index.value(),
			families.transfer.family_index.value(),
			}))
		{
			VkDeviceQueueCreateInfo queue_create_info = {};

			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family;
			queue_create_info.queueCount = 1;
			queue_create_info.pQueuePriorities = &queue_priority;

			queue_create_infos.push_back(queue_create_info);
		}

		const std::vector<const char*> device_extensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};


		VkPhysicalDeviceFeatures device_features = {};
		device_features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		create_info.pQueueCreateInfos = queue_create_infos.data();
		create_info.pEnabledFeatures = &device_features;
		create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
		create_info.ppEnabledExtensionNames = device_extensions.data();

		VkDevice device = nullptr;
		vkCreateDevice(physical_device, &create_info, nullptr, &device);
		return device;
	}

	VkSampleCountFlagBits get_max_usable_sample_count(VkPhysicalDevice physical_device)
	{
		VkPhysicalDeviceProperties physical_device_properties;
		vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

		VkSampleCountFlags counts = 
			physical_device_properties.limits.framebufferColorSampleCounts < physical_device_properties.limits.framebufferDepthSampleCounts
			? physical_device_properties.limits.framebufferColorSampleCounts
			: physical_device_properties.limits.framebufferDepthSampleCounts;

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	glm::ivec2 choose_screen_res(const glm::ivec2 screen_res, const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return {
				capabilities.currentExtent.width,
				capabilities.currentExtent.height,
			};
		}
		return {
			std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, (uint32_t)screen_res.x)),
			std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, (uint32_t)screen_res.y)),
		};
	}

	std::unique_ptr<vulkan_back_buffer> create_back_buffer(
		const vulkan_context::config& cfg,
		VkPhysicalDevice physical_device,
		VkSurfaceKHR surface,
		VkDevice device,
		const glm::ivec2& res,
		uint32_t present_queue_family,
		uint32_t graphics_queue_family)
	{
		VkSurfaceCapabilitiesKHR surface_caps = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &surface_caps);
		
		sampler samp = {
			sampler::filter::NEAREST,
			sampler::filter::NEAREST,
			sampler::address::MIRROR,
			sampler::address::MIRROR,
		};

		return vulkan_back_buffer::make({
			cfg.color_format,
			cfg.depth_format,
			samp,
			choose_screen_res(res, surface_caps),
			physical_device,
			surface,
			device,
			VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
			present_queue_family,
			graphics_queue_family,
			get_max_usable_sample_count(physical_device),
		});
	}
}

std::unique_ptr<vulkan_context> vulkan_context::make(
	const config& cfg,
	const glm::ivec2& screen_res)
{
	VkInstance instance = create_instance(cfg);
	VkDebugUtilsMessengerEXT debug_messenger = nullptr;
	
#if ATMOS_DEBUG
	debug_messenger = create_debug_messenger(cfg, instance);
#endif

	vulkan_window::config window_cfg = {};
	window_cfg.name = cfg.name;
	window_cfg.instance = instance;
	auto window = vulkan_window::make(window_cfg, screen_res);
	if (!window)
	{
		return nullptr;
	}

	queue_families families;
	VkPhysicalDevice physical_device = pick_physical_device(&families, instance, window->surface());
	VkDevice device = create_device(physical_device, families);
	if (!device)
	{
		return nullptr;
	}


	std::shared_ptr<vulkan_queue> 
		present_queue = vulkan_queue::make({ device, families.present.family_index.value(), 0 }),
		graphics_queue = vulkan_queue::make({ device, families.graphics.family_index.value(), 0 }),
		compute_queue = vulkan_queue::make({ device, families.compute.family_index.value(), 0 }),
		transfer_queue = vulkan_queue::make({ device, families.transfer.family_index.value(), 0 });
	
	vulkan_buffer_mediator::config buffer_mediator_cfg = {};
	buffer_mediator_cfg.physical_device = physical_device;
	buffer_mediator_cfg.device = device;
	buffer_mediator_cfg.present_queue = present_queue;
	buffer_mediator_cfg.graphics_queue = graphics_queue;
	buffer_mediator_cfg.compute_queue = compute_queue;
	buffer_mediator_cfg.transfer_queue = transfer_queue;
	auto buffer_mediator = vulkan_buffer_mediator::make(buffer_mediator_cfg);
	if (!buffer_mediator)
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
		graphics_queue->cfg().family_index);
	if (!back_buffer)
	{
		return nullptr;
	}

	return std::unique_ptr<vulkan_context>(
		new vulkan_context(
			cfg,
			instance,
			debug_messenger,
			physical_device,
			device,
			present_queue,
			graphics_queue,
			compute_queue,
			transfer_queue,
			std::move(buffer_mediator),
			std::move(window),
			std::move(back_buffer)));
}

const vulkan_context::config& vulkan_context::cfg() const
{
	return _cfg;
}

std::unique_ptr<program> vulkan_context::make_program(
	const program::config& base_cfg)
{
	vulkan_program::config cfg;
	COPY_TO_DERRIVED_CONFIG(base_cfg, &cfg);

	return vulkan_program::make(
		cfg);
}

std::unique_ptr<vertex_shader> vulkan_context::make_vertex_shader(
	const vertex_shader::config& cfg)
{
	return vulkan_vertex_shader::make(
		cfg,
		_state.device);
}

std::unique_ptr<fragment_shader> vulkan_context::make_fragment_shader(
	const vertex_shader::config& cfg)
{
	return vulkan_fragment_shader::make(
		cfg,
		_state.device);
}

std::unique_ptr<geometry> vulkan_context::make_geometry(
	const geometry::config& base_cfg)
{
	vulkan_geometry::config cfg = {};
	COPY_TO_DERRIVED_CONFIG(base_cfg, &cfg);

	cfg.input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	cfg.input_assembly_info.topology = to_vulkan_topology(cfg.topology);
	cfg.input_assembly_info.primitiveRestartEnable = VK_FALSE;

	return vulkan_geometry::make(cfg);
}

std::unique_ptr<vertex_buffer> vulkan_context::make_vertex_buffer(
	const vertex_buffer::config& cfg)
{
	return vulkan_vertex_buffer::make (
		cfg, 
		_buffer_mediator);
}

std::unique_ptr<index_buffer> vulkan_context::make_index_buffer(
	const index_buffer::config& cfg)
{
	return vulkan_index_buffer::make(
		cfg,
		_buffer_mediator);
}

std::unique_ptr<compute_buffer> vulkan_context::make_compute_buffer(
	const compute_buffer::config& cfg)
{
	return vulkan_compute_buffer::make(
		cfg,
		_buffer_mediator);
}
std::unique_ptr<texture2d> vulkan_context::make_texture(
	const texture2d::config& cfg)
{
	return vulkan_texture2d::make(
		cfg,
		_buffer_mediator);

}

std::unique_ptr<opaque_batch> vulkan_context::make_opaque_batch(
	const opaque_batch::config& base_cfg)
{
	vulkan_opaque_batch::config cfg = {};
	COPY_TO_DERRIVED_CONFIG(base_cfg, &cfg);
	cfg.vulkan.context = this;

	return vulkan_opaque_batch::make(
		cfg);
}

std::unique_ptr<transparent_batch> vulkan_context::make_transparent_batch(
	const transparent_batch::config& base_cfg)
{
	vulkan_transparent_batch::config cfg = {};
	COPY_TO_DERRIVED_CONFIG(base_cfg, &cfg);
	cfg.vulkan.context = this;

	return vulkan_transparent_batch::make(
		cfg);
}

const batch_constraints& vulkan_context::batch_constraints() const
{
	return _batch_constraints;
}

const material_constraints& vulkan_context::material_constraints() const
{
	return _material_constraints;
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
	const std::shared_ptr<vulkan_queue>& present_queue,
	const std::shared_ptr<vulkan_queue>& graphics_queue,
	const std::shared_ptr<vulkan_queue>& compute_queue,
	const std::shared_ptr<vulkan_queue>& transfer_queue,
	const std::shared_ptr<vulkan_buffer_mediator>& buffer_mediator,
	std::unique_ptr<vulkan_window> window,
	std::unique_ptr<vulkan_back_buffer> back_buffer)
	: _cfg(cfg)
	, _state{ instance, debug_messenger, physical_device, device }
	, _present_queue(present_queue)
	, _graphics_queue(graphics_queue)
	, _compute_queue(compute_queue)
	, _transfer_queue(transfer_queue)
	, _buffer_mediator(buffer_mediator)
	, _window(std::move(window))
	, _back_buffer(std::move(back_buffer))
	, _batch_constraints(cfg.batch_constraints)
	, _material_constraints(cfg.material_constraints)
#if ATMOS_PERFORMANCE_TRACKING
	, _renderstate_switch_metric(perf::category::SWITCH_RENDER_STATES, "Renderstates")
	, _draw_target_clears_metric(perf::category::CLEAR_DRAW_TARGET, "Draw Target Clears")
	, _draw_target_switch_metric(perf::category::SWITCH_DRAW_TARGET, "Draw Target Changes")
	, _shader_switch_metric(perf::category::SWITCH_SHADERS, "Shaders")
	, _geometry_switch_metric(perf::category::SWITCH_GEOMETRY, "geometry Count")
	, _draw_call_metric(perf::category::DRAW_CALL_COUNT, "Draw Calls")
	, _polycount_metric(perf::category::POLY_COUNT, "Polycount")
#endif
{
}

vulkan_context::auto_destroy::~auto_destroy()
{
	vkDestroyDevice(device, nullptr);

	if (debug_messenger)
	{
		DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
	}

	vkDestroyInstance(instance, nullptr);
}

vulkan_context::~vulkan_context()
{
}

VkInstance vulkan_context::instance()
{
	return _state.instance;
}

VkPhysicalDevice vulkan_context::physical_device()
{
	return _state.physical_device;
}

VkDevice vulkan_context::device()
{
	return _state.device;
}

vulkan_buffer_mediator& vulkan_context::buffer_mediator()
{
	return *_buffer_mediator;
}

void vulkan_context::resize_back_buffer(const glm::ivec2& screen_res)
{
	_back_buffer = nullptr;
	_back_buffer = ::create_back_buffer(
		_cfg,
		_state.physical_device,
		_window->surface(),
		_state.device,
		screen_res,
		_present_queue->cfg().family_index,
		_graphics_queue->cfg().family_index);
}
