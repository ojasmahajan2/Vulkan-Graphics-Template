#include "MeshBuffer.h"

MeshBuffer::MeshBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool, const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices) {
	loadMesh(vertices, indices);
	uploadBuffers(ctx, commandPool);
}

void MeshBuffer::uploadBuffers(VulkanContext& ctx, vk::raii::CommandPool& commandPool) {
	VkDeviceSize vBufferSize = sizeof(vertices[0]) * vertices.size();
	VkDeviceSize iBufferSize = sizeof(vertIndices[0]) * vertIndices.size();

	VMABuffer vStaging, iStaging;
	vStaging.allocator = ctx.allocator.handle;
	iStaging.allocator = ctx.allocator.handle;

	// Vertex Staging
	VkBufferCreateInfo vStagingInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vStagingInfo.size = vBufferSize;
	vStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingAllocInfo = {};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo vStagingAllocInfo;
	vmaCreateBuffer(ctx.allocator.handle, &vStagingInfo, &stagingAllocInfo, &vStaging.buffer, &vStaging.allocation, &vStagingAllocInfo);
	memcpy(vStagingAllocInfo.pMappedData, vertices.data(), (size_t)vBufferSize);

	// Index Staging
	VkBufferCreateInfo iStagingInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	iStagingInfo.size = iBufferSize;
	iStagingInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationInfo iStagingAllocInfo;
	vmaCreateBuffer(ctx.allocator.handle, &iStagingInfo, &stagingAllocInfo, &iStaging.buffer, &iStaging.allocation, &iStagingAllocInfo);
	memcpy(iStagingAllocInfo.pMappedData, vertIndices.data(), (size_t)iBufferSize);

	// Vertex GPU Buffer
	VkBufferCreateInfo vBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vBufferInfo.size = vBufferSize;
	vBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo gpuAllocInfo = {};
	gpuAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	vertexBuffer.allocator = ctx.allocator.handle;
	vmaCreateBuffer(ctx.allocator.handle, &vBufferInfo, &gpuAllocInfo, &vertexBuffer.buffer, &vertexBuffer.allocation, nullptr);

	// Index GPU Buffer
	VkBufferCreateInfo iBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	iBufferInfo.size = iBufferSize;
	iBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	indexBuffer.allocator = ctx.allocator.handle;
	vmaCreateBuffer(ctx.allocator.handle, &iBufferInfo, &gpuAllocInfo, &indexBuffer.buffer, &indexBuffer.allocation, nullptr);

	// Transfer
	vk::CommandBufferAllocateInfo allocInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
	vk::raii::CommandBuffer transferCmd = std::move(vk::raii::CommandBuffers(ctx.device, allocInfo).front());

	transferCmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	vk::BufferCopy vCopy(0, 0, vBufferSize);
	transferCmd.copyBuffer(vStaging.buffer, vertexBuffer.buffer, vCopy);

	vk::BufferCopy iCopy(0, 0, iBufferSize);
	transferCmd.copyBuffer(iStaging.buffer, indexBuffer.buffer, iCopy);
	transferCmd.end();

	vk::raii::Fence fence(ctx.device, vk::FenceCreateInfo());
	vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &*transferCmd, 0, nullptr);
	ctx.graphicsQueue.submit(submitInfo, *fence);

	auto waitResult = ctx.device.waitForFences(*fence, VK_TRUE, UINT64_MAX);
}

void MeshBuffer::loadMesh(const std::vector<Vertex>& newVertices, const std::vector<uint32_t>& newIndices) {
	uint32_t indexOffset = static_cast<uint32_t>(vertices.size());
	vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());

	for (uint32_t idx : newIndices) {
		vertIndices.push_back(idx + indexOffset);
	}
}