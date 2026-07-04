#pragma once
#include "VulkanContext.h"
#include "MeshBuffer.h"

struct pushConstantData {
	glm::mat4 model;
};

class Pipeline {
public:
	vk::raii::Pipeline       graphicsPipeline = nullptr;
	vk::raii::PipelineLayout layout           = nullptr;
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;

	Pipeline(VulkanContext& ctx, vk::Format swapchainImageFormat);

private:
	const std::string shaderPath = "shaders/Shaders.hlsl";

	std::string readShaderFile(const std::string& filename);
	std::vector<uint32_t> compileShadersToSPIRV(const std::string& hlslSource, const char* entryPointName);
};
