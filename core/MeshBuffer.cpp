#include "MeshBuffer.h"

MeshBuffer::MeshBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool) {
	loadMesh(rect1, rect1Indices);
//	loadMesh(rect2, rect2Indices);

	createVertexBuffer(ctx, commandPool);
	createIndexBuffer(ctx, commandPool);
}

void MeshBuffer::createVertexBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool) {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VMABuffer stagingBuffer;
	stagingBuffer.allocator = ctx.allocator.handle;

	VkBufferCreateInfo stagingBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingAllocInfo = {};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo stagingAllocationInfo;
	vmaCreateBuffer(ctx.allocator.handle, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, &stagingAllocationInfo);

	memcpy(stagingAllocationInfo.pMappedData, vertices.data(), (size_t)bufferSize);

	VkBufferCreateInfo vertexBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	vertexBufferInfo.size = bufferSize;
	vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	VmaAllocationCreateInfo vertexAllocInfo = {};
	vertexAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	vertexBuffer.allocator = ctx.allocator.handle;
	vmaCreateBuffer(ctx.allocator.handle, &vertexBufferInfo, &vertexAllocInfo, &vertexBuffer.buffer, &vertexBuffer.allocation, nullptr);

	vk::CommandBufferAllocateInfo allocInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
	vk::raii::CommandBuffer transferCmd = std::move(vk::raii::CommandBuffers(ctx.device, allocInfo).front());

	transferCmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	vk::BufferCopy copyRegion(0, 0, bufferSize);
	transferCmd.copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, copyRegion);
	transferCmd.end();

	vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &*transferCmd, 0, nullptr);
	ctx.graphicsQueue.submit(submitInfo, nullptr);

	ctx.graphicsQueue.waitIdle();
}

void MeshBuffer::createIndexBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool) {
	VkDeviceSize bufferSize = sizeof(vertIndices[0]) * vertIndices.size();

	VMABuffer stagingBuffer;
	stagingBuffer.allocator = ctx.allocator.handle;

	VkBufferCreateInfo stagingBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	stagingBufferInfo.size = bufferSize;
	stagingBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VmaAllocationCreateInfo stagingAllocInfo = {};
	stagingAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;
	stagingAllocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;

	VmaAllocationInfo stagingAllocationInfo;
	vmaCreateBuffer(ctx.allocator.handle, &stagingBufferInfo, &stagingAllocInfo, &stagingBuffer.buffer, &stagingBuffer.allocation, &stagingAllocationInfo);

	memcpy(stagingAllocationInfo.pMappedData, vertIndices.data(), (size_t)bufferSize);

	VkBufferCreateInfo indexBufferInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	indexBufferInfo.size = bufferSize;
	indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	VmaAllocationCreateInfo indexAllocInfo = {};
	indexAllocInfo.usage = VMA_MEMORY_USAGE_AUTO;

	indexBuffer.allocator = ctx.allocator.handle;
	vmaCreateBuffer(ctx.allocator.handle, &indexBufferInfo, &indexAllocInfo, &indexBuffer.buffer, &indexBuffer.allocation, nullptr);

	vk::CommandBufferAllocateInfo allocInfo(*commandPool, vk::CommandBufferLevel::ePrimary, 1);
	vk::raii::CommandBuffer transferCmd = std::move(vk::raii::CommandBuffers(ctx.device, allocInfo).front());

	transferCmd.begin({ vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	vk::BufferCopy copyRegion(0, 0, bufferSize);
	transferCmd.copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, copyRegion);
	transferCmd.end();

	vk::SubmitInfo submitInfo(0, nullptr, nullptr, 1, &*transferCmd, 0, nullptr);
	ctx.graphicsQueue.submit(submitInfo, nullptr);

	ctx.graphicsQueue.waitIdle();
}

void MeshBuffer::loadMesh(const std::vector<Vertex>& newVertices, const std::vector<uint32_t>& newIndices) {
	uint32_t indexOffset = static_cast<uint32_t>(vertices.size());
	vertices.insert(vertices.end(), newVertices.begin(), newVertices.end());

	for (uint32_t idx : newIndices) {
		vertIndices.push_back(idx + indexOffset);
	}
}