#include <vulkan/context/vulkan_context.h>

// Vulkan implementation includes - begin
#include <vulkan/buffer/vulkan_geometry.h>
#include <vulkan/buffer/vulkan_buffer_mediator.h>
#include <vulkan/buffer/vulkan_compute_buffer.h>
#include <vulkan/buffer/vulkan_index_buffer.h>
#include <vulkan/buffer/vulkan_queue.h>
#include <vulkan/buffer/vulkan_vertex_buffer.h>
#include <vulkan/defines/vulkan_includes.h>
#include <vulkan/material/vulkan_program.h>
#include <vulkan/window/vulkan_window.h>
// Vulkan implementation includes - end

#include <optional>
#include <set>
#include <vector>

using namespace igpu;

namespace
{
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


		std::vector<const char*> extensions = cfg.window->required_extensions();
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

		VkInstance instance = nullptr;
		vkCreateInstance(&create_info, nullptr, &instance);

		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

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
				LOG_CONTEXT(
					CRITICAL,
					"%s requested, but not available", layer_name);
			}
		}

		return nullptr;
	}

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
			const VkFlags flags = 0;
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
		VkFlags mask = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT;

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

				VkFlags flags = mask & properties[i].queueFlags;
				switch (family->fitness)
				{
				case queue_families::ALL:
					if (flags == mask)
					{
						family->fitness = queue_families::ALL;
						family->family_index= i;
					}
					[[fallthrough]];
				case queue_families::SOME:
					if (flags != mask && flags & family->flags)
					{
						family->fitness = queue_families::SOME;
						family->family_index = i;
					}
					[[fallthrough]];
				case queue_families::ONE:
					if (flags == family->flags)
					{
						family->fitness = queue_families::ONE;
						family->family_index = i;
					}
				}
			}

			if (false, family->family_index.has_value())
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

		VkSampleCountFlags counts = physical_device_properties.limits.framebufferColorSampleCounts;
		counts = counts > physical_device_properties.limits.framebufferDepthSampleCounts
			? physical_device_properties.limits.framebufferDepthSampleCounts
			: counts;

		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}
}

std::unique_ptr<vulkan_context> vulkan_context::make(
	const config& cfg)
{
	VkInstance instance = create_instance(cfg);
	VkSurfaceKHR surface_khr = cfg.window->make_surface(instance);
	queue_families families;

	VkPhysicalDevice physical_device = pick_physical_device(&families, instance, surface_khr);
	VkDevice device = create_device(physical_device, families);

	VkSampleCountFlagBits max_usable_sample_count = get_max_usable_sample_count(physical_device);


	vulkan_buffer_mediator::config buffer_mediator_cfg;
	buffer_mediator_cfg.present_queue = vulkan_queue::make({ device, families.present.family_index.value(), 0 });
	buffer_mediator_cfg.graphics_queue = vulkan_queue::make({ device, families.graphics.family_index.value(), 0 });
	buffer_mediator_cfg.compute_queue = vulkan_queue::make({ device, families.compute.family_index.value(), 0 });
	buffer_mediator_cfg.transfer_queue = vulkan_queue::make({ device, families.transfer.family_index.value(), 0 });

	if (auto buffer_mediator = vulkan_buffer_mediator::make(buffer_mediator_cfg))
	{
		return std::unique_ptr<vulkan_context>(
			new vulkan_context(
				cfg,
				instance,
				physical_device,
				device,
				max_usable_sample_count,
				std::move(buffer_mediator)));
	}

	return nullptr;
}

std::unique_ptr<program> vulkan_context::make_program(
	const buffer_view<uint8_t>& vertex_code,
	const buffer_view<uint8_t>& pixel_code)
{
	return vulkan_program::make(
		this,
		vertex_code,
		pixel_code);
}

std::unique_ptr<geometry> vulkan_context::make_geometry(
	const geometry::config& cfg)
{
	return vulkan_geometry::make(cfg, vertex_constraints());
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

const batch_constraints& vulkan_context::batch_constraints() const
{
	return _batch_constraints;
}

const material_constraints& vulkan_context::material_constraints() const
{
	return _material_constraints;
}

const vertex_constraints& vulkan_context::vertex_constraints() const
{
	return _vertex_constraints;
}

const vulkan_window& vulkan_context::window() const
{
	return *_cfg.window;
}

vulkan_context::vulkan_context(
	const config& cfg,
	VkInstance instance,
	VkPhysicalDevice physical_device,
	VkDevice device,
	VkSampleCountFlagBits max_usable_sample_count,
	const std::shared_ptr<vulkan_buffer_mediator>& buffer_mediator)
	: _cfg(cfg)
	, _instance(instance)
	, _physical_device(physical_device)
	, _device (device)
	, _max_usable_sample_count(max_usable_sample_count)
	, _batch_constraints(cfg.batch_constraints)
	, _material_constraints(cfg.material_constraints)
	, _vertex_constraints(cfg.vertex_constraints)
	, _buffer_mediator(std::move(buffer_mediator))
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

vulkan_context::~vulkan_context()
{
}
