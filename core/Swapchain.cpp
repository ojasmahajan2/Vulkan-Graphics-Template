#include "Swapchain.h"

Swapchain::Swapchain(VulkanContext& ctx, uint32_t width, uint32_t height) {
	createSwapchain(ctx, width, height);
	createImageViews(ctx);
}

void Swapchain::recreate(VulkanContext& ctx, uint32_t width, uint32_t height) {
	ctx.device.waitIdle();
	imageViews.clear();
	createSwapchain(ctx, width, height);
	createImageViews(ctx);
}

void Swapchain::createSwapchain(VulkanContext& ctx, uint32_t width, uint32_t height) {
	vk::SurfaceCapabilitiesKHR        capabilities = ctx.physicalDevice.getSurfaceCapabilitiesKHR(*ctx.surface);
	std::vector<vk::SurfaceFormatKHR> formats      = ctx.physicalDevice.getSurfaceFormatsKHR(*ctx.surface);
	std::vector<vk::PresentModeKHR>   presentModes = ctx.physicalDevice.getSurfacePresentModesKHR(*ctx.surface);

	vk::SurfaceFormatKHR surfaceFormat = formats[0];
	for (const auto& availableFormat : formats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
			availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			 surfaceFormat = availableFormat;
			 break;
		}
	}
	imageFormat = surfaceFormat.format;

	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
	for (const auto& availablePresentMode : presentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eFifo) {
			 presentMode = availablePresentMode;
			 break;
		}
	}

	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		extent = capabilities.currentExtent;
	}
	else {
		extent = vk::Extent2D{ width, height };
		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
	}

	uint32_t imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo(
		{}, *ctx.surface, imageCount,
		surfaceFormat.format, surfaceFormat.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);

	uint32_t queueFamilyIndices[] = { ctx.indices.graphicsFamily.value(), ctx.indices.presentFamily.value() };
	if (ctx.indices.graphicsFamily != ctx.indices.presentFamily) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}

	vk::CompositeAlphaFlagBitsKHR compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	if (capabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied) {
		compositeAlpha = vk::CompositeAlphaFlagBitsKHR::ePreMultiplied;
	}

	createInfo.preTransform 	= capabilities.currentTransform;
	createInfo.compositeAlpha 	= compositeAlpha;
	createInfo.presentMode 		= presentMode;
	createInfo.clipped 			= VK_TRUE;
	createInfo.oldSwapchain 	= *swapchain;

	swapchain = vk::raii::SwapchainKHR(ctx.device, createInfo);
	images = swapchain.getImages();
}

void Swapchain::createImageViews(VulkanContext& ctx) {
	imageViews.reserve(images.size());
	for (vk::Image image : images) {
		vk::ImageViewCreateInfo createInfo(
			{}, image, vk::ImageViewType::e2D, imageFormat,
			{}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
		);
		imageViews.emplace_back(ctx.device, createInfo);
	}
}
