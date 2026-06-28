#pragma once
#include "VulkanContext.h"

class Swapchain {
public:
	vk::raii::SwapchainKHR swapchain = nullptr;
	vk::Format imageFormat;
	vk::Extent2D extent;
	std::vector<vk::Image> images;
	std::vector<vk::raii::ImageView> imageViews;

	Swapchain(VulkanContext& ctx, uint32_t width, uint32_t height);
	void recreate(VulkanContext& ctx, uint32_t width, uint32_t height);

private:
	void createSwapchain(VulkanContext& ctx, uint32_t width, uint32_t height);
	void createImageViews(VulkanContext& ctx);
};
