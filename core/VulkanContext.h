#pragma once
#include "headers.h"

struct SDL_InitRAII {
	SDL_InitRAII(SDL_InitFlags flag) {
		if (!SDL_Init(flag)) {
			throw std::runtime_error("Failed to initiate SDL3: " + std::string(SDL_GetError()));
		}
	}

	~SDL_InitRAII() {
		SDL_Quit();
		std::cout << "SDL3 Utility cleared" << std::endl;
	}

	SDL_InitRAII(const SDL_InitRAII&) = delete;
	SDL_InitRAII& operator=(const SDL_InitRAII&) = delete;
};

struct SDL_WindowRAII {
	void operator()(SDL_Window* w) const {
		if (w) SDL_DestroyWindow(w);
	}
};

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
	SDL_InitRAII initSDL{ SDL_INIT_VIDEO };

	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	vk::raii::Queue presentQueue = nullptr;
	QueueFamilyIndices indices;
	VMAWrapper allocator;

	VulkanContext(SDL_Window* window);

private:
	void createInstance();
	void createSurface(SDL_Window* window);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createAllocator();
};
