#include "AppLayer.h"

AppLayer::AppLayer(Application& app) : Layer("AppLayer"), m_app(app) {}

void AppLayer::onAttach() {
	auto& ctx		 = m_app.getVulkanContext();
	auto& swapchain  = m_app.getSwapchain();
	auto& cmdCtx	 = m_app.getCommandContext();

	m_Pipeline = std::make_unique<Pipeline>(ctx, swapchain.imageFormat);

	std::vector<Vertex> rect = {
		{{ 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint32_t> rectIndices = {
		0, 1, 2,
		2, 3, 0
	};
	player1 = std::make_unique<MeshBuffer>(ctx, cmdCtx.commandPool, rect, rectIndices);

    std::vector<Vertex> rect2 = {
        {{ 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        {{ 0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}},
        {{-0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}},
        {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    std::vector<uint32_t> rect2Indices = {
        0, 1, 2,
        2, 3, 0
    };
    player2 = std::make_unique<MeshBuffer>(ctx, cmdCtx.commandPool, rect2, rect2Indices);

	VkBufferCreateInfo uboInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	uboInfo.size  = sizeof(CameraData);
	uboInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	cameraUBO.allocator = ctx.allocator.handle;
	vmaCreateBuffer(ctx.allocator.handle, &uboInfo, &allocInfo, &cameraUBO.buffer, &cameraUBO.allocation, nullptr);

	vk::DescriptorSetAllocateInfo dsAllocInfo(*ctx.descriptorPool, 1, &*m_Pipeline->descriptorSetLayout);
	cameraDescriptorSet = std::move(vk::raii::DescriptorSets(ctx.device, dsAllocInfo).front());

	vk::DescriptorBufferInfo bufferInfo(cameraUBO.buffer, 0, sizeof(CameraData));
	vk::WriteDescriptorSet descriptorWrite(
		*cameraDescriptorSet, 0, 0, 1, vk::DescriptorType::eUniformBuffer, nullptr, &bufferInfo, nullptr
	);
	ctx.device.updateDescriptorSets(descriptorWrite, nullptr);
}

void AppLayer::onDetach() {
	player1.reset();
    player2.reset();
	m_Pipeline.reset();
}

void AppLayer::onUpdate(float deltaTime) {
    auto& swapchain = m_app.getSwapchain();
    auto& ctx       = m_app.getVulkanContext();

    float aspectRatio = static_cast<float>(swapchain.extent.width) / static_cast<float>(swapchain.extent.height);
                        
    m_Projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    m_View       = glm::mat4(1.0f);

    CameraData camData{};
    camData.projectionView = m_Projection * m_View;
    
    void* mappedData;
    vmaMapMemory(ctx.allocator.handle, cameraUBO.allocation, &mappedData);
    memcpy(mappedData, &camData, sizeof(CameraData));
    vmaUnmapMemory(ctx.allocator.handle, cameraUBO.allocation);
}

void AppLayer::recreateSwapchain() {
    auto& window     = m_app.getWindow();
    auto& ctx        = m_app.getVulkanContext();
    auto& swapchain  = m_app.getSwapchain();
    auto& cmdCtx     = m_app.getCommandContext();

    uint32_t w = window.getWidth();
    uint32_t h = window.getHeight();

    while (w == 0 || h == 0) {
        SDL_WaitEvent(nullptr);

        window.pollEvents();
        w = window.getWidth();
        h = window.getHeight();
    }

    swapchain.recreate(ctx, w, h);
    cmdCtx.recreateSyncObjects(ctx, swapchain.images.size());
}

void AppLayer::onRender() {
    auto& ctx           = m_app.getVulkanContext();
    auto& swapchain     = m_app.getSwapchain();
    auto& cmdCtx        = m_app.getCommandContext();
    auto& window        = m_app.getWindow();

    uint32_t currentFrame = m_app.getCurrentFrame();

    auto waitResult = ctx.device.waitForFences(
        { *cmdCtx.inFlightFences[currentFrame] }, VK_TRUE, UINT64_MAX
    );
    if (waitResult != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to wait for fence!");
    }

    uint32_t imageIndex;
    try {
        auto [result, index] = swapchain.swapchain.acquireNextImage(
            UINT64_MAX, *cmdCtx.imageAvailableSemaphores[currentFrame], nullptr
        );
        imageIndex = index;

        if (result == vk::Result::eSuboptimalKHR || window.wasFramebufferResized()) {
            window.resetFramebufferResized();
            recreateSwapchain();

            auto [result2, index2] = swapchain.swapchain.acquireNextImage(
                UINT64_MAX, *cmdCtx.imageAvailableSemaphores[currentFrame], nullptr
            );
            imageIndex = index2;
        }
    }
    catch (vk::OutOfDateKHRError&) {
        window.resetFramebufferResized();
        recreateSwapchain();

        auto [result2, index2] = swapchain.swapchain.acquireNextImage(
            UINT64_MAX, *cmdCtx.imageAvailableSemaphores[currentFrame], nullptr
        );
        imageIndex = index2;
    }
    ctx.device.resetFences({ *cmdCtx.inFlightFences[currentFrame] });

    cmdCtx.commandBuffers[currentFrame].reset();
    cmdCtx.commandBuffers[currentFrame].begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

    vk::ImageMemoryBarrier drawBarrier(
        {}, vk::AccessFlagBits::eColorAttachmentWrite,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
        VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
        swapchain.images[imageIndex],
        { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    );

    cmdCtx.commandBuffers[currentFrame].pipelineBarrier(
        vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eColorAttachmentOutput,
        {}, nullptr, nullptr, drawBarrier
    );

    vk::RenderingAttachmentInfo colorAttachment(
        *swapchain.imageViews[imageIndex], vk::ImageLayout::eColorAttachmentOptimal,
        vk::ResolveModeFlagBits::eNone, nullptr, vk::ImageLayout::eUndefined,
        vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
        vk::ClearValue(vk::ClearColorValue(m_BackgroundColor))
    );

    vk::RenderingInfo renderingInfo(
        {}, vk::Rect2D({ 0, 0 }, swapchain.extent), 1, 0, 1, &colorAttachment
    );

    cmdCtx.commandBuffers[currentFrame].beginRendering(renderingInfo);

    vk::Viewport viewport(
        0.0f, 0.0f,
        (float)swapchain.extent.width, (float)swapchain.extent.height,
        0.0f, 1.0f
    );
    cmdCtx.commandBuffers[currentFrame].setViewport(0, viewport);

    vk::Rect2D scissor({ 0, 0 }, swapchain.extent);
    cmdCtx.commandBuffers[currentFrame].setScissor(0, scissor);

    // Lambda helper to draw any mesh with a given transform
    auto drawMesh = [&](const std::unique_ptr<MeshBuffer>& mesh, const glm::mat4& transform) {
        cmdCtx.commandBuffers[currentFrame].bindVertexBuffers(
            0, { mesh->vertexBuffer.buffer }, { 0 }
        );
        cmdCtx.commandBuffers[currentFrame].bindIndexBuffer(
            mesh->indexBuffer.buffer, 0, vk::IndexType::eUint32
        );

        pushConstantData pc;
        pc.model = transform;
        
        cmdCtx.commandBuffers[currentFrame].bindPipeline(
            vk::PipelineBindPoint::eGraphics, *m_Pipeline->graphicsPipeline
        );

        cmdCtx.commandBuffers[currentFrame].bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics, *m_Pipeline->layout, 0,
            { *cameraDescriptorSet }, nullptr
        );

        cmdCtx.commandBuffers[currentFrame].pushConstants<pushConstantData>(
            *m_Pipeline->layout, vk::ShaderStageFlagBits::eVertex, 0, pc
        );

        cmdCtx.commandBuffers[currentFrame].drawIndexed(
            static_cast<uint32_t>(mesh->vertIndices.size()), 1, 0, 0, 0
        );
    };

    // Calculate matrices using the stored positions
    glm::mat4 model1 = glm::translate(glm::mat4(1.0f), p1_position);
    glm::mat4 model2 = glm::translate(glm::mat4(1.0f), p2_position);
    
    // Draw both players
    drawMesh(player1, model1);
    drawMesh(player2, model2);

    cmdCtx.commandBuffers[currentFrame].endRendering();

    vk::ImageMemoryBarrier presentBarrier(
        vk::AccessFlagBits::eColorAttachmentWrite, {},
        vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
        VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
        swapchain.images[imageIndex],
        { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 }
    );

    cmdCtx.commandBuffers[currentFrame].pipelineBarrier(
        vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eBottomOfPipe,
        {}, nullptr, nullptr, presentBarrier
    );

    cmdCtx.commandBuffers[currentFrame].end();

    vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    vk::SubmitInfo submitInfo(
        1, &*cmdCtx.imageAvailableSemaphores[currentFrame], waitStages,
        1, &*cmdCtx.commandBuffers[currentFrame],
        1, &*cmdCtx.renderFinishedSemaphores[imageIndex]
    );
    ctx.graphicsQueue.submit(submitInfo, *cmdCtx.inFlightFences[currentFrame]);

    vk::PresentInfoKHR presentInfo(
        1, &*cmdCtx.renderFinishedSemaphores[imageIndex],
        1, &*swapchain.swapchain,
        &imageIndex
    );

    try {
        vk::Result presentResult = ctx.presentQueue.presentKHR(presentInfo);
        if (presentResult == vk::Result::eSuboptimalKHR || window.wasFramebufferResized()) {
            window.resetFramebufferResized();
            recreateSwapchain();
        }
    }
    catch (vk::OutOfDateKHRError&) {
        window.resetFramebufferResized();
        recreateSwapchain();
    }
}