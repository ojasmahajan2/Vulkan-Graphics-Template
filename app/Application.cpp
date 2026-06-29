#include "Application.h"

Application::Application() {
	initWindow();
	initVulkan();
}

void Application::run() {
	mainLoop();

	ctx->device.waitIdle();
}

void Application::initWindow() {
	window.reset(
		SDL_CreateWindow(
			title, width, height,
			SDL_WINDOW_VULKAN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_RESIZABLE
		)
	);

	if (!window) {
		throw std::runtime_error("Failed to create a window: " + std::string(SDL_GetError()));
	}

	SDL_AddEventWatch(resizeEventWatcher, this);
}

bool Application::resizeEventWatcher(void* userdata, SDL_Event* event) {
	if (event->type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
		auto* app = static_cast<Application*>(userdata);
		app->framebufferResized = true;
		app->drawFrame();
	}
	return true;
}

void Application::initVulkan() {
	ctx 			= std::make_unique<VulkanContext>(window.get());
	swapchain 		= std::make_unique<Swapchain>(*ctx, width, height);
	pipeline 		= std::make_unique<Pipeline>(*ctx, swapchain->imageFormat);
	commandContext 	= std::make_unique<CommandContext>(*ctx, MAX_FRAMES_IN_FLIGHT, swapchain->images.size());
	meshBuffer 		= std::make_unique<MeshBuffer>(*ctx, commandContext->commandPool);
}

void Application::recreateSwapchain() {
	int w = 0, h = 0;
	SDL_GetWindowSizeInPixels(window.get(), &w, &h);
	while (w == 0 || h == 0) {
		SDL_GetWindowSizeInPixels(window.get(), &w, &h);
		SDL_WaitEvent(nullptr);
	}

	swapchain->recreate(*ctx, w, h);
	commandContext->recreateSyncObjects(*ctx, swapchain->images.size());

	currentFrame = 0;
}

void Application::mainLoop() {
	bool running = true;
	SDL_Event event;

	int numkeys;
	const bool* state = SDL_GetKeyboardState(&numkeys);

	std::cout << std::endl;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT || state[SDL_SCANCODE_ESCAPE]) running = false;
		}

		drawFrame();
	}

	std::cout << std::endl;
}

void Application::drawFrame() {
	auto waitResult = ctx->device.waitForFences({ *commandContext->inFlightFences[currentFrame] }, VK_TRUE, UINT64_MAX);
	if (waitResult != vk::Result::eSuccess) {
		throw std::runtime_error("Failed to wait for fence!");
	}

	uint32_t imageIndex;
	try {
		auto [result, index] = swapchain->swapchain.acquireNextImage(UINT64_MAX, *commandContext->imageAvailableSemaphores[currentFrame], nullptr);
		imageIndex = index;

		if (result == vk::Result::eSuboptimalKHR || framebufferResized) {
			framebufferResized = false;
			recreateSwapchain();
			auto [result2, index2] = swapchain->swapchain.acquireNextImage(UINT64_MAX, *commandContext->imageAvailableSemaphores[currentFrame], nullptr);
			imageIndex = index2;
		}
	}
	catch (vk::OutOfDateKHRError&) {
		framebufferResized = false;
		recreateSwapchain();
		auto [result2, index2] = swapchain->swapchain.acquireNextImage(UINT64_MAX, *commandContext->imageAvailableSemaphores[currentFrame], nullptr);
		imageIndex = index2;
	}

	ctx->device.resetFences({ *commandContext->inFlightFences[currentFrame] });

	commandContext->commandBuffers[currentFrame].reset();
	commandContext->commandBuffers[currentFrame].begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

	vk::ImageMemoryBarrier drawBarrier(
		{}, vk::AccessFlagBits::eColorAttachmentWrite,
		vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
		VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
		swapchain->images[imageIndex],
		{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
	);
	commandContext->commandBuffers[currentFrame].pipelineBarrier(
		vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
		{}, nullptr, nullptr, drawBarrier
	);

	vk::RenderingAttachmentInfo colorAttachment(
		*swapchain->imageViews[imageIndex], vk::ImageLayout::eColorAttachmentOptimal,
		vk::ResolveModeFlagBits::eNone, nullptr, vk::ImageLayout::eUndefined,
		vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
		vk::ClearValue(vk::ClearColorValue(backgroundColor))
	);

	vk::RenderingInfo renderingInfo({}, vk::Rect2D({ 0, 0 }, swapchain->extent), 1, 0, 1, &colorAttachment);

	commandContext->commandBuffers[currentFrame].beginRendering(renderingInfo);

	vk::Viewport viewport(0.0f, 0.0f, (float)swapchain->extent.width, (float)swapchain->extent.height, 0.0f, 1.0f);
	commandContext->commandBuffers[currentFrame].setViewport(0, viewport);
	vk::Rect2D scissor({ 0, 0 }, swapchain->extent);
	commandContext->commandBuffers[currentFrame].setScissor(0, scissor);

	commandContext->commandBuffers[currentFrame].bindVertexBuffers(0, { meshBuffer->vertexBuffer.buffer }, { 0 });
	commandContext->commandBuffers[currentFrame].bindIndexBuffer(meshBuffer->indexBuffer.buffer, 0, vk::IndexType::eUint32);

	pushConstantData pc;
	pc.mvp = glm::mat4(1.0f);

	commandContext->commandBuffers[currentFrame].bindPipeline(vk::PipelineBindPoint::eGraphics, *pipeline->graphicsPipeline);
	commandContext->commandBuffers[currentFrame].pushConstants<pushConstantData>(*pipeline->layout, vk::ShaderStageFlagBits::eVertex, 0, pc);
	commandContext->commandBuffers[currentFrame].drawIndexed(static_cast<uint32_t>(meshBuffer->vertIndices.size()), 1, 0, 0, 0);

	commandContext->commandBuffers[currentFrame].endRendering();

	vk::ImageMemoryBarrier presentBarrier(
		vk::AccessFlagBits::eColorAttachmentWrite, {},
		vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
		VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
		swapchain->images[imageIndex],
		{ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
	);
	commandContext->commandBuffers[currentFrame].pipelineBarrier(
		vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
		{}, nullptr, nullptr, presentBarrier
	);

	commandContext->commandBuffers[currentFrame].end();

	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	vk::SubmitInfo submitInfo(
		1, &*commandContext->imageAvailableSemaphores[currentFrame], waitStages,
		1, &*commandContext->commandBuffers[currentFrame],
		1, &*commandContext->renderFinishedSemaphores[imageIndex]
	);
	ctx->graphicsQueue.submit(submitInfo, *commandContext->inFlightFences[currentFrame]);

	vk::PresentInfoKHR presentInfo(
		1, &*commandContext->renderFinishedSemaphores[imageIndex],
		1, &*swapchain->swapchain,
		&imageIndex
	);

	try {
		vk::Result presentResult = ctx->presentQueue.presentKHR(presentInfo);
		if (presentResult == vk::Result::eSuboptimalKHR || framebufferResized) {
			framebufferResized = false;
			recreateSwapchain();
		}
	}
	catch (vk::OutOfDateKHRError&) {
		framebufferResized = false;
		recreateSwapchain();
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
