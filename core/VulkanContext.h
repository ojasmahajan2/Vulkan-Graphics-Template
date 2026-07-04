#pragma once

#include "headers.h"

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() const { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

struct VMAWrapper {
	VmaAllocator handle = nullptr;
	~VMAWrapper() {
		if (handle) {
			vmaDestroyAllocator(handle);
		}
	}
};

class VulkanContext {
public:
	vk::raii::Context        context;
	vk::raii::Instance       instance       = nullptr;
	vk::raii::SurfaceKHR     surface        = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device         device         = nullptr;
	vk::raii::Queue          graphicsQueue  = nullptr;
	vk::raii::Queue          presentQueue   = nullptr;
	QueueFamilyIndices       indices;
	VMAWrapper               allocator;
	vk::raii::DescriptorPool descriptorPool = nullptr;

	VulkanContext(SDL_Window* window, const std::string& instanceName);

private:
	void createInstance(const std::string& instanceName);
	void createSurface(SDL_Window* window);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createAllocator();
	void createDescriptorPool();
};
