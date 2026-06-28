#include "CommandContext.h"

CommandContext::CommandContext(VulkanContext& ctx, uint32_t maxFramesInFlight, size_t swapchainImageCount) 
	: maxFramesInFlight(maxFramesInFlight) {
	vk::CommandPoolCreateInfo poolInfo(vk::CommandPoolCreateFlagBits::eResetCommandBuffer, ctx.indices.graphicsFamily.value());
	commandPool = vk::raii::CommandPool(ctx.device, poolInfo);

	vk::CommandBufferAllocateInfo allocInfo(*commandPool, vk::CommandBufferLevel::ePrimary, maxFramesInFlight);
	vk::raii::CommandBuffers buffers(ctx.device, allocInfo);
	for (uint32_t i = 0; i < maxFramesInFlight; i++) {
		commandBuffers.push_back(std::move(buffers[i]));
	}

	vk::SemaphoreCreateInfo semaphoreInfo;
	vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

	for (uint32_t i = 0; i < maxFramesInFlight; i++) {
		imageAvailableSemaphores.emplace_back(ctx.device, semaphoreInfo);
		inFlightFences.emplace_back(ctx.device, fenceInfo);
	}

	for (size_t i = 0; i < swapchainImageCount; i++) {
		renderFinishedSemaphores.emplace_back(ctx.device, semaphoreInfo);
	}
}

void CommandContext::recreateSyncObjects(VulkanContext& ctx, size_t swapchainImageCount) {
	imageAvailableSemaphores.clear();
	inFlightFences.clear();
	renderFinishedSemaphores.clear();

	vk::SemaphoreCreateInfo semaphoreInfo;
	vk::FenceCreateInfo fenceInfo(vk::FenceCreateFlagBits::eSignaled);

	for (uint32_t i = 0; i < maxFramesInFlight; i++) {
		imageAvailableSemaphores.emplace_back(ctx.device, semaphoreInfo);
		inFlightFences.emplace_back(ctx.device, fenceInfo);
	}

	for (size_t i = 0; i < swapchainImageCount; i++) {
		renderFinishedSemaphores.emplace_back(ctx.device, semaphoreInfo);
	}
}
