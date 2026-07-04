#pragma once

#include "../core/Application.h"
#include "../core/Pipeline.h"
#include "../core/MeshBuffer.h"

struct CameraData {
	glm::mat4 projectionView;
};

class AppLayer : public Layer {
public:
	AppLayer(Application& app);

	void onAttach()					override;
	void onDetach()					override;
	void onUpdate(float deltaTime)	override;
	void onRender()					override;

private:
	Application& m_app;

	std::unique_ptr<Pipeline>	 m_Pipeline;

	VMABuffer 				   cameraUBO;
	vk::raii::DescriptorSet    cameraDescriptorSet = nullptr;

	glm::mat4 m_Projection = glm::mat4(1.0f);
	glm::mat4 m_View = glm::mat4(1.0f);

	std::unique_ptr<MeshBuffer>  player1;
	glm::vec3 p1_position = glm::vec3(-0.5f, 0.0f, 0.0f);

	std::unique_ptr<MeshBuffer>	 player2;
	glm::vec3 p2_position = glm::vec3( 0.5f, 0.0f, 0.0f);

	std::array<float, 4> m_BackgroundColor = { 0.01f, 0.01f, 0.02f, 0.5f };

	void recreateSwapchain();
};