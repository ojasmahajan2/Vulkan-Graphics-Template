#pragma once
#include "VulkanContext.h"
#include "Swapchain.h"

class CommandContext {
public:
	vk::raii::CommandPool                commandPool = nullptr;
	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore>     imageAvailableSemaphores;
	std::vector<vk::raii::Semaphore>     renderFinishedSemaphores;
	std::vector<vk::raii::Fence>         inFlightFences;

	CommandContext(VulkanContext& ctx, uint32_t maxFramesInFlight, size_t swapchainImageCount);

	void recreateSyncObjects(VulkanContext& ctx, size_t swapchainImageCount);

private:
	uint32_t maxFramesInFlight;
};
