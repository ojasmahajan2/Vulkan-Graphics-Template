#pragma once
#include "VulkanContext.h"
#include "MeshBuffer.h"

struct pushConstantData {
	glm::mat4 mvp;
};

class Pipeline {
public:
	vk::raii::PipelineLayout layout           = nullptr;
	vk::raii::Pipeline       graphicsPipeline = nullptr;

	Pipeline(VulkanContext& ctx, vk::Format swapchainImageFormat);

private:
	const std::string shaderPath = "shaders/Shaders.hlsl";

	std::string readShaderFile(const std::string& filename);
	std::vector<uint32_t> compileShadersToSPIRV(const std::string& hlslSource, const char* entryPointName);
};
