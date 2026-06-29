#pragma once

#include "../core/VulkanContext.h"
#include "../core/Swapchain.h"
#include "../core/Pipeline.h"
#include "../core/MeshBuffer.h"
#include "../core/CommandContext.h"

class Application {
public:
	Application();
	void run();

private:
	const char* title  = "Vulkan Rectangle - TEMPLATE";
	uint32_t    width  = 1200;
	uint32_t    height = 600;

	std::unique_ptr<SDL_Window, SDL_WindowRAII> window = nullptr;
	std::unique_ptr<VulkanContext>              ctx;
	std::unique_ptr<Swapchain>                  swapchain;
	std::unique_ptr<Pipeline>                   pipeline;
	std::unique_ptr<CommandContext>             commandContext;
	std::unique_ptr<MeshBuffer>                 meshBuffer;

	const uint32_t       MAX_FRAMES_IN_FLIGHT = 2;
	uint32_t             currentFrame         = 0;
	bool                 framebufferResized   = false;
	std::array<float, 4> backgroundColor      = { 0.01f, 0.01f, 0.03f, 1.0f };

	void initWindow();
	void initVulkan();
	void mainLoop();
	void drawFrame();
	void recreateSwapchain();
	static bool resizeEventWatcher(void* userdata, SDL_Event* event);
};
