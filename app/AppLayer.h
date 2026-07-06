#pragma once

#include "../core/Application.h"
#include "../core/MeshBuffer.h"
#include "../core/Shader.h"

class AppLayer : public Layer {
public:
    AppLayer(Application& app);

    void onAttach() override;
    void onDetach() override;
    void onUpdate(float deltaTime) override;
    void onRender() override;

private:
    Application& m_app;

    std::unique_ptr<Shader> m_Shader;
    std::unique_ptr<MeshBuffer> player1;
    std::unique_ptr<MeshBuffer> player2;

    glm::mat4 m_Projection = glm::mat4(1.0f);
    glm::mat4 m_View = glm::mat4(1.0f);

    glm::vec3 p1_position = glm::vec3(-0.5f, 0.0f, 0.0f);
    glm::vec3 p2_position = glm::vec3( 0.5f, 0.0f, 0.0f);
};
