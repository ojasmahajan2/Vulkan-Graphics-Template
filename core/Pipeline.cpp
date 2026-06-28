#include "Pipeline.h"

Pipeline::Pipeline(VulkanContext& ctx, vk::Format swapchainImageFormat) {
	std::string shaderCode = readShaderFile(shaderPath);
	std::vector<uint32_t> vertSpirv = compileShadersToSPIRV(shaderCode, "vertexMain");
	std::cout << "Vertex Shader compiled successfully" << std::endl;

	std::vector<uint32_t> fragSpirv = compileShadersToSPIRV(shaderCode, "fragmentMain");
	std::cout << "Fragment Shader compiled successfully" << std::endl;

	vk::ShaderModuleCreateInfo vertInfo({}, vertSpirv);
	vk::raii::ShaderModule vertModule(ctx.device, vertInfo);

	vk::ShaderModuleCreateInfo fragInfo({}, fragSpirv);
	vk::raii::ShaderModule fragModule(ctx.device, fragInfo);

	vk::PipelineShaderStageCreateInfo shaderStages[] = {
		{{}, vk::ShaderStageFlagBits::eVertex, *vertModule, "main"},
		{{}, vk::ShaderStageFlagBits::eFragment, *fragModule, "main"}
	};

	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicStateInfo({}, dynamicStates);
	vk::PipelineViewportStateCreateInfo viewportState({}, 1, nullptr, 1, nullptr);

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo(
		{}, 1, &bindingDescription,
		static_cast<uint32_t>(attributeDescriptions.size()), attributeDescriptions.data()
	);
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);

	vk::PipelineRasterizationStateCreateInfo rasterizer(
		{}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill,
		vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise,
		VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f
	);

	vk::PipelineMultisampleStateCreateInfo multisampling({}, vk::SampleCountFlagBits::e1, VK_FALSE);
	vk::PipelineColorBlendAttachmentState colorBlendAttachment(
		VK_FALSE, vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
		vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA
	);
	vk::PipelineColorBlendStateCreateInfo colorBlending({}, VK_FALSE, vk::LogicOp::eCopy, 1, &colorBlendAttachment);

	vk::PushConstantRange pushRange(vk::ShaderStageFlagBits::eVertex, 0, sizeof(pushConstantData));
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 1, &pushRange);
	layout = vk::raii::PipelineLayout(ctx.device, pipelineLayoutInfo);

	vk::PipelineRenderingCreateInfo pipelineRenderingInfo(0, 1, &swapchainImageFormat);

	vk::GraphicsPipelineCreateInfo pipelineInfo(
		{}, 2, shaderStages, &vertexInputInfo, &inputAssembly, nullptr,
		&viewportState, &rasterizer, &multisampling, nullptr, &colorBlending,
		&dynamicStateInfo, *layout, nullptr
	);

	pipelineInfo.pNext = &pipelineRenderingInfo;

	graphicsPipeline = vk::raii::Pipeline(ctx.device, nullptr, pipelineInfo);
}

std::string Pipeline::readShaderFile(const std::string& filename) {
	std::ifstream shadersFile(filename);
	if (!shadersFile.is_open()) {
		throw std::runtime_error("Failed to open Shader file: " + filename);
	}
	std::stringstream buffer;
	buffer << shadersFile.rdbuf();
	return buffer.str();
}

std::vector<uint32_t> Pipeline::compileShadersToSPIRV(const std::string& shaderSrc, const char* entryPointName) {
	Slang::ComPtr<slang::IGlobalSession> globalSession;
	slang::createGlobalSession(globalSession.writeRef());

	slang::TargetDesc targetDesc = {};
	targetDesc.format = SLANG_SPIRV;
	targetDesc.profile = globalSession->findProfile("sm_6_5");

	slang::SessionDesc sessionDesc = {};
	sessionDesc.targets = &targetDesc;
	sessionDesc.targetCount = 1;

	Slang::ComPtr<slang::ISession> session;
	globalSession->createSession(sessionDesc, session.writeRef());

	Slang::ComPtr<slang::IBlob> diagnosticBlob;

	slang::IModule* module = session->loadModuleFromSourceString(
		"Shaders",
		"path.slang",
		shaderSrc.c_str(),
		diagnosticBlob.writeRef()
	);

	if (diagnosticBlob) {
		std::cout << "Slang Compiler message: " << (const char*)diagnosticBlob->getBufferPointer() << "\n";
	}
	
	if (!module) {
		throw std::runtime_error("Failed to load Slang shader module!");
	}

	Slang::ComPtr<slang::IEntryPoint> entryPoint;
	module->findEntryPointByName(entryPointName, entryPoint.writeRef());

	if (!entryPoint) {
		throw std::runtime_error("Failed to find entry point '" + std::string(entryPointName) + "' in Shader file!");
	}

	std::vector<slang::IComponentType*> components = { module, entryPoint };
	Slang::ComPtr<slang::IComponentType> program;
	session->createCompositeComponentType(
		components.data(), components.size(), program.writeRef(), diagnosticBlob.writeRef()
	);

	Slang::ComPtr<slang::IBlob> spirvBlob;
	program->getEntryPointCode(0, 0, spirvBlob.writeRef(), diagnosticBlob.writeRef());

	if (!spirvBlob) {
		throw std::runtime_error("Failed to generate SPIR-V bytecode!");
	}

	const uint32_t* codeStart = (const uint32_t*)spirvBlob->getBufferPointer();
	size_t codeSize = spirvBlob->getBufferSize() / sizeof(uint32_t);

	return std::vector<uint32_t>(codeStart, codeStart + codeSize);
}
