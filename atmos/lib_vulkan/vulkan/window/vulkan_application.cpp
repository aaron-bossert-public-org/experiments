//
//#include <vulkan/defines/vulkan_includes.h>
//
//#define GLM_ENABLE_EXPERIMENTAL 1
//#include <glm/glm.hpp>
//#include <glm/gtx/hash.hpp>
//
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <tinyobjloader/tiny_obj_loader.h>
//
//#include <iostream>
//#include <fstream>
//#include <stdexcept>
//#include <algorithm>
//#include <chrono>
//#include <vector>
//#include <cstring>
//#include <cstdlib>
//#include <array>
//#include <optional>
//#include <set>
//#include <thread>
//#include <unordered_map>
//
//
//const std::string MODEL_PATH = "cooked_assets/models/chalet.obj";
//const std::string TEXTURE_PATH = "cooked_assets/textures/chalet.jpg";
//
//const int MAX_FRAMES_IN_FLIGHT = 2;
//
//const std::vector<const char*> validation_layers = {
//	"VK_LAYER_LUNARG_standard_validation"
//};
//
//const std::vector<const char*> device_extensions = {
//	VK_KHR_SWAPCHAIN_EXTENSION_NAME
//};
//
//#ifdef NDEBUG
//const bool enable_validation_layers = false;
//#else
//const bool enable_validation_layers = true;
//#endif
//
//VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info, const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger) {
//	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
//	if (func != nullptr) {
//		return func(instance, p_create_info, p_allocator, p_debug_messenger);
//	}
//	else {
//		return VK_ERROR_EXTENSION_NOT_PRESENT;
//	}
//}
//
//void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debug_messenger, const VkAllocationCallbacks* p_allocator) {
//	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//	if (func != nullptr) {
//		func(instance, debug_messenger, p_allocator);
//	}
//}
//
//struct QueueFamilyIndices {
//	std::optional<uint32_t> graphics_family;
//	std::optional<uint32_t> present_family;
//
//	bool is_complete() {
//		return graphics_family.has_value() && present_family.has_value();
//	}
//};
//
//struct SwapChainSupportDetails {
//	VkSurfaceCapabilitiesKHR capabilities;
//	std::vector<VkSurfaceFormatKHR> formats;
//	std::vector<VkPresentModeKHR> present_modes;
//};
//
//struct Vertex {
//	glm::vec3 pos;
//	glm::vec3 color;
//	glm::vec2 tex_coord;
//
//	static VkVertexInputBindingDescription get_binding_description() {
//		VkVertexInputBindingDescription binding_description = {};
//		binding_description.binding = 0;
//		binding_description.stride = sizeof(Vertex);
//		binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//		return binding_description;
//	}
//
//	static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions() {
//		std::array<VkVertexInputAttributeDescription, 3> attribute_descriptions = {};
//
//		attribute_descriptions[0].binding = 0;
//		attribute_descriptions[0].location = 0;
//		attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//		attribute_descriptions[0].offset = offsetof(Vertex, pos);
//
//		attribute_descriptions[1].binding = 0;
//		attribute_descriptions[1].location = 1;
//		attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
//		attribute_descriptions[1].offset = offsetof(Vertex, color);
//
//		attribute_descriptions[2].binding = 0;
//		attribute_descriptions[2].location = 2;
//		attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
//		attribute_descriptions[2].offset = offsetof(Vertex, tex_coord);
//
//		return attribute_descriptions;
//	}
//
//	bool operator==(const Vertex& other) const {
//		return pos == other.pos && color == other.color && tex_coord == other.tex_coord;
//	}
//};
//
//namespace std {
//	template<> struct hash<Vertex> {
//		size_t operator()(Vertex const& vertex) const {
//			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.tex_coord) << 1);
//		}
//	};
//}
//
//struct UniformBufferObject {
//	alignas(16) glm::mat4 model;
//	alignas(16) glm::mat4 view;
//	alignas(16) glm::mat4 proj;
//};
//
//#include <vulkan/window/vulkan_window.h>
//
//using namespace igpu;
//
//class HelloTriangleApplication {
//
//
//private:
//
//	vulkan_window* _window;
//
//	VkInstance _instance;
//	VkDebugUtilsMessengerEXT _debug_messenger;
//	VkSurfaceKHR _surface;
//
//	VkPhysicalDevice _physical_device = VK_NULL_HANDLE;
//	VkSampleCountFlagBits _msaa_samples = VK_SAMPLE_COUNT_1_BIT;
//	VkDevice _device;
//
//	VkQueue _graphics_queue;
//	VkQueue _present_queue;
//
//	VkSwapchainKHR _swap_chain;
//	std::vector<VkImage> _swap_chain_images;
//	VkFormat _swap_chain_image_format;
//	VkExtent2D _swap_chain_extent;
//	std::vector<VkImageView> _swap_chain_image_views;
//	std::vector<VkFramebuffer> _swap_chain_framebuffers;
//
//	VkRenderPass _render_pass;
//	VkDescriptorSetLayout _descriptor_set_layout;
//	VkPipelineLayout _pipeline_layout;
//	VkPipeline _graphics_pipeline;
//
//	VkCommandPool _command_pool;
//
//	VkImage _color_image;
//	VkDeviceMemory _color_image_memory;
//	VkImageView _color_image_view;
//
//	VkImage _depth_image;
//	VkDeviceMemory _depth_image_memory;
//	VkImageView _depth_image_view;
//
//	uint32_t _mip_levels;
//	VkImage _texture_image;
//	VkDeviceMemory _texture_image_memory;
//	VkImageView _texture_image_view;
//	VkSampler _texture_sampler;
//
//	std::vector<Vertex> _vertices;
//	std::vector<uint32_t> _indices;
//	VkBuffer _vertex_buffer;
//	VkDeviceMemory _vertex_buffer_memory;
//	VkBuffer _index_buffer;
//	VkDeviceMemory _index_buffer_memory;
//
//	std::vector<VkBuffer> _uniform_buffers;
//	std::vector<VkDeviceMemory> _uniform_buffers_memory;
//
//	VkDescriptorPool _descriptor_pool;
//	std::vector<VkDescriptorSet> _descriptor_sets;
//
//	std::vector<VkCommandBuffer> _command_buffers;
//
//	std::vector<VkSemaphore> _image_available_semaphores;
//	std::vector<VkSemaphore> _render_finished_semaphores;
//	std::vector<VkFence> _in_flight_fences;
//	size_t _current_frame = 0;
//
//	bool _framebuffer_resized = false;
//public:
//
//	void framebuffer_resized()
//	{
//		_framebuffer_resized = true;
//	}
//
//	void init_vulkan(vulkan_window* window) {
//		_window = window;
//		create_instance();
//		setup_debug_messenger();
//		create_surface();
//		pick_physical_device();
//		create_logical_device();
//		create_swap_chain();
//		create_image_views();
//		create_render_pass();
//		create_descriptor_set_layout();
//		create_graphics_pipeline();
//		create_command_pool();
//		create_color_resources();
//		create_depth_resources();
//		create_framebuffers();
//		create_texture_image();
//		create_texture_image_view();
//		create_texture_sampler();
//		load_model();
//		create_vertex_buffer();
//		create_index_buffer();
//		create_uniform_buffers();
//		create_descriptor_pool();
//		create_descriptor_sets();
//		create_command_buffers();
//		create_sync_objects();
//	}
//
//	bool main_loop() {
//		
//		if (_window->poll_events()) {
//			draw_frame();
//			return true;
//		}
//		return false;
//	}
//
//	void cleanup() {
//
//
//		vkDeviceWaitIdle(_device);
//
//		cleanup_swap_chain();
//
//		vkDestroySampler(_device, _texture_sampler, nullptr);
//		vkDestroyImageView(_device, _texture_image_view, nullptr);
//
//		vkDestroyImage(_device, _texture_image, nullptr);
//		vkFreeMemory(_device, _texture_image_memory, nullptr);
//
//		vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
//
//		vkDestroyDescriptorSetLayout(_device, _descriptor_set_layout, nullptr);
//
//		for (size_t i = 0; i < _swap_chain_images.size(); i++) {
//			vkDestroyBuffer(_device, _uniform_buffers[i], nullptr);
//			vkFreeMemory(_device, _uniform_buffers_memory[i], nullptr);
//		}
//
//		vkDestroyBuffer(_device, _index_buffer, nullptr);
//		vkFreeMemory(_device, _index_buffer_memory, nullptr);
//
//		vkDestroyBuffer(_device, _vertex_buffer, nullptr);
//		vkFreeMemory(_device, _vertex_buffer_memory, nullptr);
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			vkDestroySemaphore(_device, _render_finished_semaphores[i], nullptr);
//			vkDestroySemaphore(_device, _image_available_semaphores[i], nullptr);
//			vkDestroyFence(_device, _in_flight_fences[i], nullptr);
//		}
//
//		vkDestroyCommandPool(_device, _command_pool, nullptr);
//
//		vkDestroyDevice(_device, nullptr);
//
//		if (enable_validation_layers) {
//			DestroyDebugUtilsMessengerEXT(_instance, _debug_messenger, nullptr);
//		}
//
//		vkDestroySurfaceKHR(_instance, _surface, nullptr);
//		vkDestroyInstance(_instance, nullptr);
//		
//		_window = nullptr;
//	}
//
//	private:
//
//	void cleanup_swap_chain() {
//		vkDestroyImageView(_device, _depth_image_view, nullptr);
//		vkDestroyImage(_device, _depth_image, nullptr);
//		vkFreeMemory(_device, _depth_image_memory, nullptr);
//
//		vkDestroyImageView(_device, _color_image_view, nullptr);
//		vkDestroyImage(_device, _color_image, nullptr);
//		vkFreeMemory(_device, _color_image_memory, nullptr);
//
//		for (auto framebuffer : _swap_chain_framebuffers) {
//			vkDestroyFramebuffer(_device, framebuffer, nullptr);
//		}
//
//		vkFreeCommandBuffers(_device, _command_pool, static_cast<uint32_t>(_command_buffers.size()), _command_buffers.data());
//
//		vkDestroyPipeline(_device, _graphics_pipeline, nullptr);
//		vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
//		vkDestroyRenderPass(_device, _render_pass, nullptr);
//
//		for (auto image_view : _swap_chain_image_views) {
//			vkDestroyImageView(_device, image_view, nullptr);
//		}
//
//		vkDestroySwapchainKHR(_device, _swap_chain, nullptr);
//	}
//
//	void recreate_swap_chain() {
//		glm::ivec2 window_res;
//
//		while (window_res.x == 0 || window_res.y == 0) {	
//			_window->poll_events();
//			window_res = _window->res();
//			std::this_thread::sleep_for(std::chrono::seconds(1));
//		}
//
//		vkDeviceWaitIdle(_device);
//
//		cleanup_swap_chain();
//
//		create_swap_chain();
//		create_image_views();
//		create_render_pass();
//		create_graphics_pipeline();
//		create_color_resources();
//		create_depth_resources();
//		create_framebuffers();
//		create_command_buffers();
//	}
//
//	void create_instance() {
//		if (enable_validation_layers && !check_validation_layer_support()) {
//			throw std::runtime_error("validation layers requested, but not available!");
//		}
//
//		VkApplicationInfo app_info = {};
//		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
//		app_info.pApplicationName = "Hello Triangle";
//		app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
//		app_info.pEngineName = "No Engine";
//		app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
//		app_info.apiVersion = VK_API_VERSION_1_0;
//
//		VkInstanceCreateInfo create_info = {};
//		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
//		create_info.pApplicationInfo = &app_info;
//
//		auto extensions = get_required_extensions();
//		create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
//		create_info.ppEnabledExtensionNames = extensions.data();
//
//		if (enable_validation_layers) {
//			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
//			create_info.ppEnabledLayerNames = validation_layers.data();
//		}
//		else {
//			create_info.enabledLayerCount = 0;
//		}
//
//		if (vkCreateInstance(&create_info, nullptr, &_instance) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create instance!");
//		}
//	}
//
//	void setup_debug_messenger() {
//		if (!enable_validation_layers) return;
//
//		VkDebugUtilsMessengerCreateInfoEXT create_info = {};
//		create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
//		create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
//		create_info.pfnUserCallback = debug_callback;
//
//		if (CreateDebugUtilsMessengerEXT(_instance, &create_info, nullptr, &_debug_messenger) != VK_SUCCESS) {
//			throw std::runtime_error("failed to set up debug messenger!");
//		}
//	}
//
//	void create_surface() {
//		_window->make_surface(_instance, &_surface);
//	}
//
//	void pick_physical_device() {
//		uint32_t device_count = 0;
//		vkEnumeratePhysicalDevices(_instance, &device_count, nullptr);
//
//		if (device_count == 0) {
//			throw std::runtime_error("failed to find GPUs with Vulkan support!");
//		}
//
//		std::vector<VkPhysicalDevice> devices(device_count);
//		vkEnumeratePhysicalDevices(_instance, &device_count, devices.data());
//
//		for (const auto& device : devices) {
//			if (is_device_suitable(device)) {
//				_physical_device = device;
//				_msaa_samples = get_max_usable_sample_count();
//				break;
//			}
//		}
//
//		if (_physical_device == VK_NULL_HANDLE) {
//			throw std::runtime_error("failed to find a suitable GPU!");
//		}
//	}
//
//	void create_logical_device() {
//		QueueFamilyIndices indices = find_queue_families(_physical_device);
//
//		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
//		std::set<uint32_t> unique_queue_families = { indices.graphics_family.value(), indices.present_family.value() };
//
//		float queue_priority = 1.0f;
//		for (uint32_t queue_family : unique_queue_families) {
//			VkDeviceQueueCreateInfo queue_create_info = {};
//			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
//			queue_create_info.queueFamilyIndex = queue_family;
//			queue_create_info.queueCount = 1;
//			queue_create_info.pQueuePriorities = &queue_priority;
//			queue_create_infos.push_back(queue_create_info);
//		}
//
//		VkPhysicalDeviceFeatures device_features = {};
//		device_features.samplerAnisotropy = VK_TRUE;
//
//		VkDeviceCreateInfo create_info = {};
//		create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
//
//		create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
//		create_info.pQueueCreateInfos = queue_create_infos.data();
//
//		create_info.pEnabledFeatures = &device_features;
//
//		create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
//		create_info.ppEnabledExtensionNames = device_extensions.data();
//
//		if (enable_validation_layers) {
//			create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
//			create_info.ppEnabledLayerNames = validation_layers.data();
//		}
//		else {
//			create_info.enabledLayerCount = 0;
//		}
//
//		if (vkCreateDevice(_physical_device, &create_info, nullptr, &_device) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create logical device!");
//		}
//
//		vkGetDeviceQueue(_device, indices.graphics_family.value(), 0, &_graphics_queue);
//		vkGetDeviceQueue(_device, indices.present_family.value(), 0, &_present_queue);
//	}
//
//	void create_swap_chain() {
//		SwapChainSupportDetails swap_chain_support = query_swap_chain_support(_physical_device);
//
//		VkSurfaceFormatKHR surface_format = choose_swap_surface_format(swap_chain_support.formats);
//		VkPresentModeKHR present_mode = choose_swap_present_mode(swap_chain_support.present_modes);
//		VkExtent2D extent = choose_swap_extent(swap_chain_support.capabilities);
//
//		uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
//		if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
//			image_count = swap_chain_support.capabilities.maxImageCount;
//		}
//
//		VkSwapchainCreateInfoKHR create_info = {};
//		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
//		create_info.surface = _surface;
//
//		create_info.minImageCount = image_count;
//		create_info.imageFormat = surface_format.format;
//		create_info.imageColorSpace = surface_format.colorSpace;
//		create_info.imageExtent = extent;
//		create_info.imageArrayLayers = 1;
//		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
//
//		QueueFamilyIndices indices = find_queue_families(_physical_device);
//		uint32_t queue_family_indices[] = { indices.graphics_family.value(), indices.present_family.value() };
//
//		if (indices.graphics_family != indices.present_family) {
//			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
//			create_info.queueFamilyIndexCount = 2;
//			create_info.pQueueFamilyIndices = queue_family_indices;
//		}
//		else {
//			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
//		}
//
//		create_info.preTransform = swap_chain_support.capabilities.currentTransform;
//		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
//		create_info.presentMode = present_mode;
//		create_info.clipped = VK_TRUE;
//
//		if (vkCreateSwapchainKHR(_device, &create_info, nullptr, &_swap_chain) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create swap chain!");
//		}
//
//		vkGetSwapchainImagesKHR(_device, _swap_chain, &image_count, nullptr);
//		_swap_chain_images.resize(image_count);
//		vkGetSwapchainImagesKHR(_device, _swap_chain, &image_count, _swap_chain_images.data());
//
//		_swap_chain_image_format = surface_format.format;
//		_swap_chain_extent = extent;
//	}
//
//	void create_image_views() {
//		_swap_chain_image_views.resize(_swap_chain_images.size());
//
//		for (uint32_t i = 0; i < _swap_chain_images.size(); i++) {
//			_swap_chain_image_views[i] = create_image_view(_swap_chain_images[i], _swap_chain_image_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
//		}
//	}
//
//	void create_render_pass() {
//		VkAttachmentDescription color_attachment = {};
//		color_attachment.format = _swap_chain_image_format;
//		color_attachment.samples = _msaa_samples;
//		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//		VkAttachmentDescription depth_attachment = {};
//		depth_attachment.format = find_depth_format();
//		depth_attachment.samples = _msaa_samples;
//		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
//		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//		VkAttachmentDescription color_attachment_resolve = {};
//		color_attachment_resolve.format = _swap_chain_image_format;
//		color_attachment_resolve.samples = VK_SAMPLE_COUNT_1_BIT;
//		color_attachment_resolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		color_attachment_resolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//		color_attachment_resolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//		color_attachment_resolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//		color_attachment_resolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		color_attachment_resolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
//
//		VkAttachmentReference color_attachment_ref = {};
//		color_attachment_ref.attachment = 0;
//		color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//		VkAttachmentReference depth_attachment_ref = {};
//		depth_attachment_ref.attachment = 1;
//		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
//
//		VkAttachmentReference color_attachment_resolve_ref = {};
//		color_attachment_resolve_ref.attachment = 2;
//		color_attachment_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//		VkSubpassDescription subpass = {};
//		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//		subpass.colorAttachmentCount = 1;
//		subpass.pColorAttachments = &color_attachment_ref;
//		subpass.pDepthStencilAttachment = &depth_attachment_ref;
//		subpass.pResolveAttachments = &color_attachment_resolve_ref;
//
//		VkSubpassDependency dependency = {};
//		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//		dependency.dstSubpass = 0;
//		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		dependency.srcAccessMask = 0;
//		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//
//		std::array<VkAttachmentDescription, 3> attachments = { color_attachment, depth_attachment, color_attachment_resolve };
//		VkRenderPassCreateInfo render_pass_info = {};
//		render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//		render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
//		render_pass_info.pAttachments = attachments.data();
//		render_pass_info.subpassCount = 1;
//		render_pass_info.pSubpasses = &subpass;
//		render_pass_info.dependencyCount = 1;
//		render_pass_info.pDependencies = &dependency;
//
//		if (vkCreateRenderPass(_device, &render_pass_info, nullptr, &_render_pass) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create render pass!");
//		}
//	}
//
//	void create_descriptor_set_layout() {
//		VkDescriptorSetLayoutBinding ubo_layout_binding = {};
//		ubo_layout_binding.binding = 0;
//		ubo_layout_binding.descriptorCount = 1;
//		ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		ubo_layout_binding.pImmutableSamplers = nullptr;
//		ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//
//		VkDescriptorSetLayoutBinding sampler_layout_binding = {};
//		sampler_layout_binding.binding = 1;
//		sampler_layout_binding.descriptorCount = 1;
//		sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		sampler_layout_binding.pImmutableSamplers = nullptr;
//		sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
//
//		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };
//		VkDescriptorSetLayoutCreateInfo layout_info = {};
//		layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//		layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
//		layout_info.pBindings = bindings.data();
//
//		if (vkCreateDescriptorSetLayout(_device, &layout_info, nullptr, &_descriptor_set_layout) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create descriptor set layout!");
//		}
//	}
//
//	void create_graphics_pipeline() {
//		auto vert_shader_code = read_file("cooked_assets/shaders/vert.spv");
//		auto frag_shader_code = read_file("cooked_assets/shaders/frag.spv");
//
//		VkShaderModule vert_shader_module = create_shader_module(vert_shader_code);
//		VkShaderModule frag_shader_module = create_shader_module(frag_shader_code);
//
//		VkPipelineShaderStageCreateInfo vert_shader_stage_info = {};
//		vert_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		vert_shader_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
//		vert_shader_stage_info.module = vert_shader_module;
//		vert_shader_stage_info.pName = "main";
//
//		VkPipelineShaderStageCreateInfo frag_shader_stage_info = {};
//		frag_shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//		frag_shader_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//		frag_shader_stage_info.module = frag_shader_module;
//		frag_shader_stage_info.pName = "main";
//
//		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_stage_info, frag_shader_stage_info };
//
//		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
//		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
//
//		auto binding_description = Vertex::get_binding_description();
//		auto attribute_descriptions = Vertex::get_attribute_descriptions();
//
//		vertex_input_info.vertexBindingDescriptionCount = 1;
//		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_descriptions.size());
//		vertex_input_info.pVertexBindingDescriptions = &binding_description;
//		vertex_input_info.pVertexAttributeDescriptions = attribute_descriptions.data();
//
//		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
//		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
//		input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//		input_assembly.primitiveRestartEnable = VK_FALSE;
//
//		VkViewport viewport = {};
//		viewport.x = 0.0f;
//		viewport.y = 0.0f;
//		viewport.width = (float)_swap_chain_extent.width;
//		viewport.height = (float)_swap_chain_extent.height;
//		viewport.minDepth = 0.0f;
//		viewport.maxDepth = 1.0f;
//
//		VkRect2D scissor = {};
//		scissor.offset = { 0, 0 };
//		scissor.extent = _swap_chain_extent;
//
//		VkPipelineViewportStateCreateInfo viewport_state = {};
//		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
//		viewport_state.viewportCount = 1;
//		viewport_state.pViewports = &viewport;
//		viewport_state.scissorCount = 1;
//		viewport_state.pScissors = &scissor;
//
//		VkPipelineRasterizationStateCreateInfo rasterizer = {};
//		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
//		rasterizer.depthClampEnable = VK_FALSE;
//		rasterizer.rasterizerDiscardEnable = VK_FALSE;
//		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
//		rasterizer.lineWidth = 1.0f;
//		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
//		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
//		rasterizer.depthBiasEnable = VK_FALSE;
//
//		VkPipelineMultisampleStateCreateInfo multisampling = {};
//		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
//		multisampling.sampleShadingEnable = VK_FALSE;
//		multisampling.rasterizationSamples = _msaa_samples;
//
//		VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
//		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
//		depth_stencil.depthTestEnable = VK_TRUE;
//		depth_stencil.depthWriteEnable = VK_TRUE;
//		depth_stencil.depthCompareOp = VK_COMPARE_OP_LESS;
//		depth_stencil.depthBoundsTestEnable = VK_FALSE;
//		depth_stencil.stencilTestEnable = VK_FALSE;
//
//		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
//		color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//		color_blend_attachment.blendEnable = VK_FALSE;
//
//		VkPipelineColorBlendStateCreateInfo color_blending = {};
//		color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
//		color_blending.logicOpEnable = VK_FALSE;
//		color_blending.logicOp = VK_LOGIC_OP_COPY;
//		color_blending.attachmentCount = 1;
//		color_blending.pAttachments = &color_blend_attachment;
//		color_blending.blendConstants[0] = 0.0f;
//		color_blending.blendConstants[1] = 0.0f;
//		color_blending.blendConstants[2] = 0.0f;
//		color_blending.blendConstants[3] = 0.0f;
//
//		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
//		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
//		pipeline_layout_info.setLayoutCount = 1;
//		pipeline_layout_info.pSetLayouts = &_descriptor_set_layout;
//
//		if (vkCreatePipelineLayout(_device, &pipeline_layout_info, nullptr, &_pipeline_layout) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create pipeline layout!");
//		}
//
//		VkGraphicsPipelineCreateInfo pipeline_info = {};
//		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
//		pipeline_info.stageCount = 2;
//		pipeline_info.pStages = shader_stages;
//		pipeline_info.pVertexInputState = &vertex_input_info;
//		pipeline_info.pInputAssemblyState = &input_assembly;
//		pipeline_info.pViewportState = &viewport_state;
//		pipeline_info.pRasterizationState = &rasterizer;
//		pipeline_info.pMultisampleState = &multisampling;
//		pipeline_info.pDepthStencilState = &depth_stencil;
//		pipeline_info.pColorBlendState = &color_blending;
//		pipeline_info.layout = _pipeline_layout;
//		pipeline_info.renderPass = _render_pass;
//		pipeline_info.subpass = 0;
//		pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
//
//		if (vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &_graphics_pipeline) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create graphics pipeline!");
//		}
//
//		vkDestroyShaderModule(_device, frag_shader_module, nullptr);
//		vkDestroyShaderModule(_device, vert_shader_module, nullptr);
//	}
//
//	void create_framebuffers() {
//		_swap_chain_framebuffers.resize(_swap_chain_image_views.size());
//
//		for (size_t i = 0; i < _swap_chain_image_views.size(); i++) {
//			std::array<VkImageView, 3> attachments = {
//				_color_image_view,
//				_depth_image_view,
//				_swap_chain_image_views[i]
//			};
//
//			VkFramebufferCreateInfo framebuffer_info = {};
//			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//			framebuffer_info.renderPass = _render_pass;
//			framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
//			framebuffer_info.pAttachments = attachments.data();
//			framebuffer_info.width = _swap_chain_extent.width;
//			framebuffer_info.height = _swap_chain_extent.height;
//			framebuffer_info.layers = 1;
//
//			if (vkCreateFramebuffer(_device, &framebuffer_info, nullptr, &_swap_chain_framebuffers[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to create framebuffer!");
//			}
//		}
//	}
//
//	void create_command_pool() {
//		QueueFamilyIndices queue_family_indices = find_queue_families(_physical_device);
//
//		VkCommandPoolCreateInfo pool_info = {};
//		pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
//		pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
//
//		if (vkCreateCommandPool(_device, &pool_info, nullptr, &_command_pool) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create graphics command pool!");
//		}
//	}
//
//	void create_color_resources() {
//		VkFormat color_format = _swap_chain_image_format;
//
//		create_image(_swap_chain_extent.width, _swap_chain_extent.height, 1, _msaa_samples, color_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _color_image, _color_image_memory);
//		_color_image_view = create_image_view(_color_image, color_format, VK_IMAGE_ASPECT_COLOR_BIT, 1);
//
//		transition_image_layout(_color_image, color_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 1);
//	}
//
//	void create_depth_resources() {
//		VkFormat depth_format = find_depth_format();
//
//		create_image(_swap_chain_extent.width, _swap_chain_extent.height, 1, _msaa_samples, depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _depth_image, _depth_image_memory);
//		_depth_image_view = create_image_view(_depth_image, depth_format, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
//
//		transition_image_layout(_depth_image, depth_format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
//	}
//
//	VkFormat find_supported_format(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
//		for (VkFormat format : candidates) {
//			VkFormatProperties props;
//			vkGetPhysicalDeviceFormatProperties(_physical_device, format, &props);
//
//			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
//				return format;
//			}
//			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
//				return format;
//			}
//		}
//
//		throw std::runtime_error("failed to find supported format!");
//	}
//
//	VkFormat find_depth_format() {
//		return find_supported_format(
//			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
//			VK_IMAGE_TILING_OPTIMAL,
//			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
//		);
//	}
//
//	bool has_stencil_component(VkFormat format) {
//		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
//	}
//
//	void create_texture_image() {
//		int tex_width, tex_height, tex_channels;
//		stbi* pixels = stbi_load(TEXTURE_PATH.c_str(), &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
//		VkDeviceSize image_size = tex_width * tex_height * 4;
//		_mip_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(tex_width, tex_height)))) + 1;
//
//		if (!pixels) {
//			throw std::runtime_error("failed to load texture image!");
//		}
//
//		VkBuffer staging_buffer;
//		VkDeviceMemory staging_buffer_memory;
//		create_buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
//
//		void* data;
//		vkMapMemory(_device, staging_buffer_memory, 0, image_size, 0, &data);
//		memcpy(data, pixels, static_cast<size_t>(imageSize));
//		vkUnmapMemory(_device, staging_buffer_memory);
//
//		stbi_image_free(pixels);
//
//		create_image(tex_width, tex_height, _mip_levels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _texture_image, _texture_image_memory);
//
//		transitionImageLayout(_texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, _mip_levels);
//		copyBufferToImage(stagingBuffer, _texture_image, static_cast<uint32_t>(tex_width), static_cast<uint32_t>(tex_height));
//		//transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
//
//		vkDestroyBuffer(_device, staging_buffer, nullptr);
//		vkFreeMemory(_device, staging_buffer_memory, nullptr);
//
//		generateMipmaps(_texture_image, VK_FORMAT_R8G8B8A8_UNORM, tex_width, tex_height, _mip_levels);
//	}
//
//	void generate_mipmaps(VkImage image, VkFormat image_format, int32_t tex_width, int32_t tex_height, uint32_t mip_levels) {
//		// Check if image format supports linear blitting
//		VkFormatProperties format_properties;
//		vkGetPhysicalDeviceFormatProperties(_physical_device, image_format, &formatProperties);
//
//		if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
//			throw std::runtime_error("texture image format does not support linear blitting!");
//		}
//
//		VkCommandBuffer command_buffer = begin_single_time_commands();
//
//		VkImageMemoryBarrier barrier = {};
//		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//		barrier.image = image;
//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		barrier.subresourceRange.baseArrayLayer = 0;
//		barrier.subresourceRange.layerCount = 1;
//		barrier.subresourceRange.levelCount = 1;
//
//		int32_t mip_width = tex_width;
//		int32_t mip_height = tex_height;
//
//		for (uint32_t i = 1; i < mip_levels; i++) {
//			barrier.subresourceRange.baseMipLevel = i - 1;
//			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//
//			vkCmdPipelineBarrier(commandBuffer,
//				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
//				0, nullptr,
//				0, nullptr,
//				1, &barrier);
//
//			VkImageBlit blit = {};
//			blit.srcOffsets[0] = { 0, 0, 0 };
//			blit.srcOffsets[1] = { mip_width, mip_height, 1 };
//			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//			blit.srcSubresource.mipLevel = i - 1;
//			blit.srcSubresource.baseArrayLayer = 0;
//			blit.srcSubresource.layerCount = 1;
//			blit.dstOffsets[0] = { 0, 0, 0 };
//			blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1 };
//			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//			blit.dstSubresource.mipLevel = i;
//			blit.dstSubresource.baseArrayLayer = 0;
//			blit.dstSubresource.layerCount = 1;
//
//			vkCmdBlitImage(commandBuffer,
//				image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
//				image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//				1, &blit,
//				VK_FILTER_LINEAR);
//
//			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
//			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			vkCmdPipelineBarrier(commandBuffer,
//				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
//				0, nullptr,
//				0, nullptr,
//				1, &barrier);
//
//			if (mipWidth > 1) mip_width /= 2;
//			if (mipHeight > 1) mip_height /= 2;
//		}
//
//		barrier.subresourceRange.baseMipLevel = mip_levels - 1;
//		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//		vkCmdPipelineBarrier(commandBuffer,
//			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
//			0, nullptr,
//			0, nullptr,
//			1, &barrier);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//
//	VkSampleCountFlagBits get_max_usable_sample_count() {
//		VkPhysicalDeviceProperties physical_device_properties;
//		vkGetPhysicalDeviceProperties(_physical_device, &physicalDeviceProperties);
//
//		VkSampleCountFlags counts = std::min(physicalDeviceProperties.limits.framebufferColorSampleCounts, physical_device_properties.limits.framebufferDepthSampleCounts);
//		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
//		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
//		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
//		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
//		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
//		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
//
//		return VK_SAMPLE_COUNT_1_BIT;
//	}
//
//	void create_texture_image_view() {
//		_texture_image_view = create_image_view(_texture_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, _mip_levels);
//	}
//
//	void create_texture_sampler() {
//		VkSamplerCreateInfo sampler_info = {};
//		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//		samplerInfo.magFilter = VK_FILTER_LINEAR;
//		samplerInfo.minFilter = VK_FILTER_LINEAR;
//		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.anisotropyEnable = VK_TRUE;
//		samplerInfo.maxAnisotropy = 16;
//		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//		samplerInfo.unnormalizedCoordinates = VK_FALSE;
//		samplerInfo.compareEnable = VK_FALSE;
//		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//		samplerInfo.minLod = 0;
//		samplerInfo.maxLod = static_cast<float>(_mip_levels);
//		samplerInfo.mipLodBias = 0;
//
//		if (vkCreateSampler(_device, &samplerInfo, nullptr, &_texture_sampler) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create texture sampler!");
//		}
//	}
//
//	VkImageView create_image_view(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels) {
//		VkImageViewCreateInfo view_info = {};
//		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//		viewInfo.image = image;
//		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//		viewInfo.format = format;
//		viewInfo.subresourceRange.aspectMask = aspect_flags;
//		viewInfo.subresourceRange.baseMipLevel = 0;
//		viewInfo.subresourceRange.levelCount = mip_levels;
//		viewInfo.subresourceRange.baseArrayLayer = 0;
//		viewInfo.subresourceRange.layerCount = 1;
//
//		VkImageView image_view;
//		if (vkCreateImageView(_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create texture image view!");
//		}
//
//		return image_view;
//	}
//
//	void create_image(uint32_t width, uint32_t height, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {
//		VkImageCreateInfo image_info = {};
//		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//		image_info.imageType = VK_IMAGE_TYPE_2D;
//		image_info.extent.width = width;
//		image_info.extent.height = height;
//		image_info.extent.depth = 1;
//		image_info.mipLevels = mip_levels;
//		image_info.arrayLayers = 1;
//		image_info.format = format;
//		image_info.tiling = tiling;
//		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		image_info.usage = usage;
//		image_info.samples = num_samples;
//		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateImage(_device, &image_info, nullptr, &image) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create image!");
//		}
//
//		VkMemoryRequirements mem_requirements;
//		vkGetImageMemoryRequirements(_device, image, &memRequirements);
//
//		VkMemoryAllocateInfo alloc_info = {};
//		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		alloc_info.allocationSize = mem_requirements.size;
//		alloc_info.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(_device, &alloc_info, nullptr, &imageMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate image memory!");
//		}
//
//		vkBindImageMemory(_device, image, image_memory, 0);
//	}
//
//	void transition_image_layout(VkImage image, VkFormat format, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_levels) {
//		VkCommandBuffer command_buffer = begin_single_time_commands();
//
//		VkImageMemoryBarrier barrier = {};
//		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//		barrier.oldLayout = old_layout;
//		barrier.newLayout = new_layout;
//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.image = image;
//
//		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//
//			if (hasStencilComponent(format)) {
//				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//			}
//		}
//		else {
//			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		}
//
//		barrier.subresourceRange.baseMipLevel = 0;
//		barrier.subresourceRange.levelCount = mip_levels;
//		barrier.subresourceRange.baseArrayLayer = 0;
//		barrier.subresourceRange.layerCount = 1;
//
//		VkPipelineStageFlags source_stage;
//		VkPipelineStageFlags destination_stage;
//
//		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//		}
//		else {
//			throw std::invalid_argument("unsupported layout transition!");
//		}
//
//		vkCmdPipelineBarrier(
//			commandBuffer,
//			sourceStage, destination_stage,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &barrier
//		);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//
//	void copy_buffer_to_image(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
//		VkCommandBuffer command_buffer = begin_single_time_commands();
//
//		VkBufferImageCopy region = {};
//		region.bufferOffset = 0;
//		region.bufferRowLength = 0;
//		region.bufferImageHeight = 0;
//		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		region.imageSubresource.mipLevel = 0;
//		region.imageSubresource.baseArrayLayer = 0;
//		region.imageSubresource.layerCount = 1;
//		region.imageOffset = { 0, 0, 0 };
//		region.imageExtent = {
//			width,
//			height,
//			1
//		};
//
//		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//
//	void load_model() {
//		tinyobj::attrib_t attrib;
//		std::vector<tinyobj::shape_t> shapes;
//		std::vector<tinyobj::material_t> materials;
//		std::string warn, err;
//
//		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
//			throw std::runtime_error(warn + err);
//		}
//
//		std::unordered_map<Vertex, uint32_t> unique_vertices = {};
//
//		for (const auto& shape : shapes) {
//			for (const auto& index : shape.mesh.indices) {
//				Vertex vertex = {};
//
//				vertex.pos = {
//					attrib.vertices[3 * index.vertex_index + 0],
//					attrib.vertices[3 * index.vertex_index + 1],
//					attrib.vertices[3 * index.vertex_index + 2]
//				};
//
//				vertex.tex_coord = {
//					attrib.texcoords[2 * index.texcoord_index + 0],
//					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
//				};
//
//				vertex.color = { 1.0f, 1.0f, 1.0f };
//
//				if (uniqueVertices.count(vertex) == 0) {
//					uniqueVertices[vertex] = static_cast<uint32_t>(_vertices.size());
//					_vertices.push_back(vertex);
//				}
//
//				_indices.push_back(uniqueVertices[vertex]);
//			}
//		}
//	}
//
//	void create_vertex_buffer() {
//		VkDeviceSize buffer_size = sizeof(_vertices[0]) * _vertices.size();
//
//		VkBuffer staging_buffer;
//		VkDeviceMemory staging_buffer_memory;
//		create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
//
//		void* data;
//		vkMapMemory(_device, staging_buffer_memory, 0, buffer_size, 0, &data);
//		memcpy(data, _vertices.data(), (size_t)bufferSize);
//		vkUnmapMemory(_device, staging_buffer_memory);
//
//		create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _vertex_buffer, _vertex_buffer_memory);
//
//		copyBuffer(stagingBuffer, _vertex_buffer, buffer_size);
//
//		vkDestroyBuffer(_device, staging_buffer, nullptr);
//		vkFreeMemory(_device, staging_buffer_memory, nullptr);
//	}
//
//	void create_index_buffer() {
//		VkDeviceSize buffer_size = sizeof(_indices[0]) * _indices.size();
//
//		VkBuffer staging_buffer;
//		VkDeviceMemory staging_buffer_memory;
//		create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_buffer_memory);
//
//		void* data;
//		vkMapMemory(_device, staging_buffer_memory, 0, buffer_size, 0, &data);
//		memcpy(data, _indices.data(), (size_t)bufferSize);
//		vkUnmapMemory(_device, staging_buffer_memory);
//
//		create_buffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _index_buffer, _index_buffer_memory);
//
//		copyBuffer(stagingBuffer, _index_buffer, buffer_size);
//
//		vkDestroyBuffer(_device, staging_buffer, nullptr);
//		vkFreeMemory(_device, staging_buffer_memory, nullptr);
//	}
//
//	void create_uniform_buffers() {
//		VkDeviceSize buffer_size = sizeof(UniformBufferObject);
//
//		_uniform_buffers.resize(_swap_chain_images.size());
//		_uniform_buffers_memory.resize(_swap_chain_images.size());
//
//		for (size_t i = 0; i < _swap_chain_images.size(); i++) {
//			create_buffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _uniform_buffers[i], _uniform_buffers_memory[i]);
//		}
//	}
//
//	void create_descriptor_pool() {
//		std::array<VkDescriptorPoolSize, 2> pool_sizes = {};
//		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		poolSizes[0].descriptorCount = static_cast<uint32_t>(_swap_chain_images.size());
//		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		poolSizes[1].descriptorCount = static_cast<uint32_t>(_swap_chain_images.size());
//
//		VkDescriptorPoolCreateInfo pool_info = {};
//		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//		pool_info.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//		pool_info.pPoolSizes = pool_sizes.data();
//		pool_info.maxSets = static_cast<uint32_t>(_swap_chain_images.size());
//
//		if (vkCreateDescriptorPool(_device, &pool_info, nullptr, &_descriptor_pool) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create descriptor pool!");
//		}
//	}
//
//	void create_descriptor_sets() {
//		std::vector<VkDescriptorSetLayout> layouts(_swap_chain_images.size(), _descriptor_set_layout);
//		VkDescriptorSetAllocateInfo alloc_info = {};
//		alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
//		alloc_info.descriptorPool = _descriptor_pool;
//		alloc_info.descriptorSetCount = static_cast<uint32_t>(_swap_chain_images.size());
//		alloc_info.pSetLayouts = layouts.data();
//
//		_descriptor_sets.resize(_swap_chain_images.size());
//		if (vkAllocateDescriptorSets(_device, &alloc_info, _descriptor_sets.data()) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate descriptor sets!");
//		}
//
//		for (size_t i = 0; i < _swap_chain_images.size(); i++) {
//			VkDescriptorBufferInfo buffer_info = {};
//			buffer_info.buffer = _uniform_buffers[i];
//			buffer_info.offset = 0;
//			buffer_info.range = sizeof(UniformBufferObject);
//
//			VkDescriptorImageInfo image_info = {};
//			image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//			image_info.imageView = _texture_image_view;
//			image_info.sampler = _texture_sampler;
//
//			std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};
//
//			descriptor_writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			descriptor_writes[0].dstSet = _descriptor_sets[i];
//			descriptor_writes[0].dstBinding = 0;
//			descriptor_writes[0].dstArrayElement = 0;
//			descriptor_writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//			descriptor_writes[0].descriptorCount = 1;
//			descriptor_writes[0].pBufferInfo = &buffer_info;
//
//			descriptor_writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
//			descriptor_writes[1].dstSet = _descriptor_sets[i];
//			descriptor_writes[1].dstBinding = 1;
//			descriptor_writes[1].dstArrayElement = 0;
//			descriptor_writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//			descriptor_writes[1].descriptorCount = 1;
//			descriptor_writes[1].pImageInfo = &image_info;
//
//			vkUpdateDescriptorSets(_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
//		}
//	}
//
//	void create_buffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory) {
//		VkBufferCreateInfo buffer_info = {};
//		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		buffer_info.size = size;
//		buffer_info.usage = usage;
//		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create buffer!");
//		}
//
//		VkMemoryRequirements mem_requirements;
//		vkGetBufferMemoryRequirements(_device, buffer, &memRequirements);
//
//		VkMemoryAllocateInfo alloc_info = {};
//		alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		alloc_info.allocationSize = mem_requirements.size;
//		alloc_info.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(_device, &alloc_info, nullptr, &bufferMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindBufferMemory(_device, buffer, buffer_memory, 0);
//	}
//
//	VkCommandBuffer begin_single_time_commands() {
//		VkCommandBufferAllocateInfo alloc_info = {};
//		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		alloc_info.commandPool = _command_pool;
//		alloc_info.commandBufferCount = 1;
//
//		VkCommandBuffer command_buffer;
//		vkAllocateCommandBuffers(_device, &alloc_info, &commandBuffer);
//
//		VkCommandBufferBeginInfo begin_info = {};
//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//		return command_buffer;
//	}
//
//	void end_single_time_commands(VkCommandBuffer command_buffer) {
//		vkEndCommandBuffer(commandBuffer);
//
//		VkSubmitInfo submit_info = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffer;
//
//		vkQueueSubmit(_graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
//		vkQueueWaitIdle(_graphics_queue);
//
//		vkFreeCommandBuffers(_device, _command_pool, 1, &commandBuffer);
//	}
//
//	void copy_buffer(VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size) {
//		VkCommandBuffer command_buffer = begin_single_time_commands();
//
//		VkBufferCopy copy_region = {};
//		copyRegion.size = size;
//		vkCmdCopyBuffer(commandBuffer, src_buffer, dst_buffer, 1, &copyRegion);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//
//	uint32_t find_memory_type(uint32_t type_filter, VkMemoryPropertyFlags properties) {
//		VkPhysicalDeviceMemoryProperties mem_properties;
//		vkGetPhysicalDeviceMemoryProperties(_physical_device, &memProperties);
//
//		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
//			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//				return i;
//			}
//		}
//
//		throw std::runtime_error("failed to find suitable memory type!");
//	}
//
//	void create_command_buffers() {
//		_command_buffers.resize(_swap_chain_framebuffers.size());
//
//		VkCommandBufferAllocateInfo alloc_info = {};
//		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		alloc_info.commandPool = _command_pool;
//		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		alloc_info.commandBufferCount = (uint32_t)_command_buffers.size();
//
//		if (vkAllocateCommandBuffers(_device, &alloc_info, _command_buffers.data()) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate command buffers!");
//		}
//
//		for (size_t i = 0; i < _command_buffers.size(); i++) {
//			VkCommandBufferBeginInfo begin_info = {};
//			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
//
//			if (vkBeginCommandBuffer(_command_buffers[i], &beginInfo) != VK_SUCCESS) {
//				throw std::runtime_error("failed to begin recording command buffer!");
//			}
//
//			VkRenderPassBeginInfo render_pass_info = {};
//			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//			render_pass_info.renderPass = _render_pass;
//			render_pass_info.framebuffer = _swap_chain_framebuffers[i];
//			render_pass_info.renderArea.offset = { 0, 0 };
//			render_pass_info.renderArea.extent = _swap_chain_extent;
//
//			std::array<VkClearValue, 2> clear_values = {};
//			clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
//			clearValues[1].depth_stencil = { 1.0f, 0 };
//
//			render_pass_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
//			render_pass_info.pClearValues = clear_values.data();
//
//			vkCmdBeginRenderPass(_command_buffers[i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);
//
//			vkCmdBindPipeline(_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _graphics_pipeline);
//
//			VkBuffer vertex_buffers[] = { _vertex_buffer };
//			VkDeviceSize offsets[] = { 0 };
//			vkCmdBindVertexBuffers(_command_buffers[i], 0, 1, vertex_buffers, offsets);
//
//			vkCmdBindIndexBuffer(_command_buffers[i], _index_buffer, 0, VK_INDEX_TYPE_UINT32);
//
//			vkCmdBindDescriptorSets(_command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout, 0, 1, &_descriptor_sets[i], 0, nullptr);
//
//			vkCmdDrawIndexed(_command_buffers[i], static_cast<uint32_t>(_indices.size()), 1, 0, 0, 0);
//
//			vkCmdEndRenderPass(_command_buffers[i]);
//
//			if (vkEndCommandBuffer(_command_buffers[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to record command buffer!");
//			}
//		}
//	}
//
//	void create_sync_objects() {
//		_image_available_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		_render_finished_semaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);
//
//		VkSemaphoreCreateInfo semaphore_info = {};
//		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//		VkFenceCreateInfo fence_info = {};
//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			if (vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_image_available_semaphores[i]) != VK_SUCCESS ||
//				vkCreateSemaphore(_device, &semaphoreInfo, nullptr, &_render_finished_semaphores[i]) != VK_SUCCESS ||
//				vkCreateFence(_device, &fenceInfo, nullptr, &_in_flight_fences[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to create synchronization objects for a frame!");
//			}
//		}
//	}
//
//	void update_uniform_buffer(uint32_t current_image) {
//		static auto start_time = std::chrono::high_resolution_clock::now();
//
//		auto current_time = std::chrono::high_resolution_clock::now();
//		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - start_time).count();
//
//		UniformBufferObject ubo = {};
//		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//		ubo.proj = glm::perspective(glm::radians(45.0f), _swap_chain_extent.width / (float)_swap_chain_extent.height, 0.1f, 10.0f);
//		ubo.proj[1][1] *= -1;
//
//		void* data;
//		vkMapMemory(_device, _uniform_buffers_memory[currentImage], 0, sizeof(ubo), 0, &data);
//		memcpy(data, &ubo, sizeof(ubo));
//		vkUnmapMemory(_device, _uniform_buffers_memory[currentImage]);
//	}
//
//	void draw_frame() {
//		vkWaitForFences(_device, 1, &_in_flight_fences[_current_frame], VK_TRUE, std::numeric_limits<uint64_t>::max());
//
//		uint32_t image_index = 0;
//
//		try {
//			vkAcquireNextImageKHR(_device, _swap_chain, std::numeric_limits<uint64_t>::max(), _image_available_semaphores[_current_frame], VK_NULL_HANDLE, &imageIndex);
//		}
//		catch (const vulkan_result_exception& e) {
//
//			if (e.res() == VK_ERROR_OUT_OF_DATE_KHR) {
//				recreateSwapChain();
//				return;
//			}
//			else if (e.res() != VK_SUBOPTIMAL_KHR) {
//				std::rethrow_exception(std::current_exception());
//			}
//		}
//
//		vkQueueWaitIdle(_present_queue);
//
//		updateUniformBuffer(imageIndex);
//
//		VkSubmitInfo submit_info = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//		VkSemaphore wait_semaphores[] = { _image_available_semaphores[_current_frame] };
//		VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
//		submitInfo.waitSemaphoreCount = 1;
//		submitInfo.pWaitSemaphores = wait_semaphores;
//		submitInfo.pWaitDstStageMask = wait_stages;
//
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &_command_buffers[imageIndex];
//
//		VkSemaphore signal_semaphores[] = { _render_finished_semaphores[_current_frame] };
//		submitInfo.signalSemaphoreCount = 1;
//		submitInfo.pSignalSemaphores = signal_semaphores;
//
//		vkResetFences(_device, 1, &_in_flight_fences[_current_frame]);
//
//		if (vkQueueSubmit(_graphics_queue, 1, &submitInfo, _in_flight_fences[_current_frame]) != VK_SUCCESS) {
//			throw std::runtime_error("failed to submit draw command buffer!");
//		}
//
//		VkPresentInfoKHR present_info = {};
//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//		presentInfo.waitSemaphoreCount = 1;
//		presentInfo.pWaitSemaphores = signal_semaphores;
//
//		VkSwapchainKHR swap_chains[] = { _swap_chain };
//		presentInfo.swapchainCount = 1;
//		presentInfo.pSwapchains = swap_chains;
//
//		presentInfo.pImageIndices = &imageIndex;
//
//		try {
//			vkQueuePresentKHR(_present_queue, &presentInfo);
//		}
//		catch (const vulkan_result_exception& e) {
//
//			if (e.res() == VK_ERROR_OUT_OF_DATE_KHR ||
//				e.res() == VK_SUBOPTIMAL_KHR) {
//				_framebuffer_resized = true;
//			}
//			else {
//				std::rethrow_exception(std::current_exception());
//			}
//		}
//
//		if (_framebuffer_resized) {
//			_framebuffer_resized = false;
//			recreateSwapChain();
//		}
//
//		_current_frame = (_current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
//	}
//
//	VkShaderModule create_shader_module(const std::vector<char>& code) {
//		VkShaderModuleCreateInfo create_info = {};
//		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
//		create_info.codeSize = code.size();
//		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
//
//		VkShaderModule shader_module;
//		if (vkCreateShaderModule(_device, &create_info, nullptr, &shaderModule) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create shader module!");
//		}
//
//		return shader_module;
//	}
//
//	VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) {
//		if (availableFormats.size() == 1 && available_formats[0].format == VK_FORMAT_UNDEFINED) {
//			return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
//		}
//
//		for (const auto& available_format : available_formats) {
//			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
//				return available_format;
//			}
//		}
//
//		return available_formats[0];
//	}
//
//	VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& available_present_modes) {
//		VkPresentModeKHR best_mode = VK_PRESENT_MODE_FIFO_KHR;
//
//		for (const auto& available_present_mode : available_present_modes) {
//			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
//				return available_present_mode;
//			}
//			else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
//				bestMode = available_present_mode;
//			}
//		}
//
//		return best_mode;
//	}
//
//	VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities) {
//		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
//			return capabilities.currentExtent;
//		}
//		else {
//			size_t width = _window->width();
//			size_t height = _window->height();
//
//			VkExtent2D actual_extent = {
//				static_cast<uint32_t>(width),
//				static_cast<uint32_t>(height)
//			};
//
//			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actual_extent.width));
//			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actual_extent.height));
//
//			return actual_extent;
//		}
//	}
//
//	SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device) {
//		SwapChainSupportDetails details;
//
//		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, _surface, &details.capabilities);
//
//		uint32_t format_count;
//		vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, nullptr);
//
//		if (formatCount != 0) {
//			details.formats.resize(formatCount);
//			vkGetPhysicalDeviceSurfaceFormatsKHR(device, _surface, &formatCount, details.formats.data());
//		}
//
//		uint32_t present_mode_count;
//		vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, nullptr);
//
//		if (presentModeCount != 0) {
//			details.presentModes.resize(presentModeCount);
//			vkGetPhysicalDeviceSurfacePresentModesKHR(device, _surface, &presentModeCount, details.presentModes.data());
//		}
//
//		return details;
//	}
//
//	bool is_device_suitable(VkPhysicalDevice device) {
//		QueueFamilyIndices indices = find_queue_families(device);
//
//		bool extensions_supported = check_device_extension_support(device);
//
//		bool swap_chain_adequate = false;
//		if (extensionsSupported) {
//			SwapChainSupportDetails swap_chain_support = query_swap_chain_support(device);
//			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
//		}
//
//		VkPhysicalDeviceFeatures supported_features;
//		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
//
//		return indices.isComplete() && extensions_supported && swap_chain_adequate  && supported_features.samplerAnisotropy;
//	}
//
//	bool check_device_extension_support(VkPhysicalDevice device) {
//		uint32_t extension_count;
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
//
//		std::vector<VkExtensionProperties> available_extensions(extensionCount);
//		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, available_extensions.data());
//
//		std::set<std::string> required_extensions(deviceExtensions.begin(), device_extensions.end());
//
//		for (const auto& extension : available_extensions) {
//			requiredExtensions.erase(extension.extensionName);
//		}
//
//		return required_extensions.empty();
//	}
//
//	QueueFamilyIndices find_queue_families(VkPhysicalDevice device) {
//		QueueFamilyIndices indices;
//
//		uint32_t queue_family_count = 0;
//		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
//
//		std::vector<VkQueueFamilyProperties> queue_families(queueFamilyCount);
//		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queue_families.data());
//
//		int i = 0;
//		for (const auto& queue_family : queue_families) {
//			if (queueFamily.queueCount > 0 && queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
//				indices.graphics_family = i;
//			}
//
//			VkBool32 present_support = false;
//			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, _surface, &presentSupport);
//
//			if (queueFamily.queueCount > 0 && present_support) {
//				indices.present_family = i;
//			}
//
//			if (indices.isComplete()) {
//				break;
//			}
//
//			i++;
//		}
//
//		return indices;
//	}
//
//	std::vector<const char*> get_required_extensions() {
//		
//		auto extensions = _window->required_extensions();
//		
//		if (enable_validation_layers) {
//			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//		}
//
//		return extensions;
//	}
//
//	bool check_validation_layer_support() {
//		uint32_t layer_count;
//		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
//
//		std::vector<VkLayerProperties> available_layers(layerCount);
//		vkEnumerateInstanceLayerProperties(&layerCount, available_layers.data());
//
//		for (const char* layer_name : validation_layers) {
//			bool layer_found = false;
//
//			for (const auto& layer_properties : available_layers) {
//				if (strcmp(layerName, layer_properties.layerName) == 0) {
//					layerFound = true;
//					break;
//				}
//			}
//
//			if (!layerFound) {
//				return false;
//			}
//		}
//
//		return true;
//	}
//
//	static std::vector<char> read_file(const std::string& filename) {
//		std::ifstream file(filename, std::ios::ate | std::ios::binary);
//
//		if (!file.is_open()) {
//			throw std::runtime_error("failed to open file!");
//		}
//
//		size_t file_size = (size_t)file.tellg();
//		std::vector<char> buffer(fileSize);
//
//		file.seekg(0);
//		file.read(buffer.data(), file_size);
//
//		file.close();
//
//		return buffer;
//	}
//
//	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT /* message_severity */, VkDebugUtilsMessageTypeFlagsEXT /* message_type */, const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data, void* /* p_user_data */) {
//		std::cerr << "validation layer: " << p_callback_data->pMessage << std::endl;
//
//		return VK_FALSE;
//	}
//};
//
//HelloTriangleApplication hello_triangle_application;
//
//void port_example();
//
//std::unique_ptr<application> application::make(vulkan_window* window)
//{
//	port_example();
//
//	try {
//		helloTriangleApplication.init_vulkan(window);
//	}
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//	}
//
//	return std::unique_ptr<application>(new application());
//}
//
//application::~application()
//{
//	helloTriangleApplication.cleanup();
//}
//
//bool application::present()
//{
//	try {
//		return hello_triangle_application.mainLoop();
//	}
//	catch (const std::exception& e) {
//		std::cerr << e.what() << std::endl;
//	}
//
//	return false;
//}
