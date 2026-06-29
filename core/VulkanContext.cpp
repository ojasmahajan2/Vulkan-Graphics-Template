#define VMA_IMPLEMENTATION
#include "VulkanContext.h"

VulkanContext::VulkanContext(SDL_Window* window) {
	createInstance();
	createSurface(window);
	pickPhysicalDevice();
	createLogicalDevice();
	createAllocator();
}

void VulkanContext::createInstance() {
	vk::ApplicationInfo appInfo(
		"Vulkan Subject",
		VK_MAKE_VERSION(1, 0, 0),
		"Basic Engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_4
	);

	uint32_t sdlExtensionCount = 0;
	const char* const* sdlExtensions     = SDL_Vulkan_GetInstanceExtensions(&sdlExtensionCount);

	if (!sdlExtensions) {
		throw std::runtime_error("Failed to get SDL Vulkan extensions: " + std::string(SDL_GetError()));
	}

	std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

#ifdef NDEBUG
	std::vector<const char*> validationLayers = {};
#else
	std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
#endif

	vk::InstanceCreateInfo createInfo(
		{},
		&appInfo,
		validationLayers,
		extensions
	);

	instance = vk::raii::Instance(context, createInfo);
}

void VulkanContext::createSurface(SDL_Window* window) {
	VkSurfaceKHR c_surface;

	if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &c_surface)) {
		throw std::runtime_error("Failed to create SDL Vulkan Surface: " + std::string(SDL_GetError()));
	}

	surface = vk::raii::SurfaceKHR(instance, c_surface);
}

void VulkanContext::pickPhysicalDevice() {
	vk::raii::PhysicalDevices devices(instance);
	if (devices.empty()) {
		throw std::runtime_error("No GPUs found with Vulkan compatibility!");
	}

	std::cout << "Found " << devices.size() << " GPUs with Vulkan support" << std::endl;

	for (const auto& d : devices) {
		vk::PhysicalDeviceProperties properties = d.getProperties();

		if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			physicalDevice = d;
			std::cout << "Selected GPU: " << properties.deviceName << std::endl;

			break;
		}
	}

	if (!*physicalDevice) {
		physicalDevice = devices.front();
		std::cout << "Fallback GPU Selected: " << physicalDevice.getProperties().deviceName << std::endl;
	}
}

void VulkanContext::createLogicalDevice() {
	std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
		}

		if (physicalDevice.getSurfaceSupportKHR(i, *surface)) {
			indices.presentFamily = i;
		}
		if (indices.isComplete()) break;
		i++;
	}

	if (!indices.isComplete()) {
		throw std::runtime_error("Failed to find queue families!");
	}

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		queueCreateInfos.push_back(vk::DeviceQueueCreateInfo(
			{}, queueFamily, 1, &queuePriority
		));
	}

	std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vk::PhysicalDeviceVulkan14Features features14;
	features14.maintenance5 = VK_TRUE;

	vk::PhysicalDeviceVulkan13Features features13;
	features13.dynamicRendering = VK_TRUE;
	features13.pNext = &features14;

	vk::PhysicalDeviceVulkan11Features features11;
	features11.shaderDrawParameters = VK_TRUE;
	features11.pNext = &features13;

	vk::PhysicalDeviceFeatures2 deviceFeatures;
	deviceFeatures.features.fillModeNonSolid = VK_TRUE;
	deviceFeatures.pNext = &features11;

	vk::DeviceCreateInfo createInfo(
		{}, queueCreateInfos, {}, deviceExtensions, nullptr
	);
	createInfo.pNext = &deviceFeatures;

	device = vk::raii::Device(physicalDevice, createInfo);

	graphicsQueue = vk::raii::Queue(device, indices.graphicsFamily.value(), 0);
	presentQueue = vk::raii::Queue(device, indices.presentFamily.value(), 0);
}

void VulkanContext::createAllocator() {
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_4;
	allocatorInfo.physicalDevice   = *physicalDevice;
	allocatorInfo.device           = *device;
	allocatorInfo.instance         = *instance;

	if (vmaCreateAllocator(&allocatorInfo, &allocator.handle) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create VMA Allocator!");
	}
}
