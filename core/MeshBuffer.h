#pragma once
#include "VulkanContext.h"

struct VMABuffer {
	VkBuffer buffer = nullptr;
	VmaAllocation allocation = nullptr;
	VmaAllocator allocator = nullptr;

	VMABuffer() = default;

	VMABuffer(const VMABuffer&) = delete;
	VMABuffer& operator=(const VMABuffer&) = delete;

	~VMABuffer() {
		if (buffer && allocation && allocator) {
			vmaDestroyBuffer(allocator, buffer, allocation);
		}
	}
};

struct Vertex {
	glm::vec2 position;
	glm::vec3 color;

	static vk::VertexInputBindingDescription getBindingDescription() {
		return vk::VertexInputBindingDescription(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
	}

	static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<vk::VertexInputAttributeDescription, 2> attributes = {};
		attributes[0] = vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32Sfloat, offsetof(Vertex, position));
		attributes[1] = vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color));
		return attributes;
	}
};

class MeshBuffer {
public:
	VMABuffer vertexBuffer;
	VMABuffer indexBuffer;

	MeshBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool);
	std::vector<Vertex> vertices = {};
	std::vector<uint32_t> vertIndices = {};

	void loadMesh(const std::vector<Vertex>& newVertices, const std::vector<uint32_t>& newIndices);

private:

	void createVertexBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool);
	void createIndexBuffer(VulkanContext& ctx, vk::raii::CommandPool& commandPool);

	std::vector<Vertex> rect1 = {
		{{ 0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f},	{0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f},	{0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f},	{1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint32_t> rect1Indices = {
		0, 1, 2,
		2, 3, 0
	};

	/*std::vector<Vertex> rect2 = {
		{{ 0.5f, -0.5f},	{1.0f, 0.0f, 0.0f}},
		{{ 0.5f,  0.5f},		{0.0f, 1.0f, 0.0f}},
		{{-0.5f,  0.5f},	{0.0f, 0.0f, 1.0f}},
		{{-0.5f, -0.5f},	{1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint32_t> rect2Indices = {
		0, 1, 2,
		2, 3, 0
	};*/
};
