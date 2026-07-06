#include "AppLayer.h"

AppLayer::AppLayer(Application& app) : Layer("AppLayer"), m_app(app) {}

void AppLayer::onAttach() {
    m_Shader = std::make_unique<Shader>("shaders/vert.glsl", "shaders/frag.glsl");

    std::vector<float> rect = {
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 1.0f
    };
    std::vector<uint32_t> rectIndices = {
        0, 1, 2,
        2, 3, 0
    };
    player1 = std::make_unique<MeshBuffer>(rect, rectIndices);
    player2 = std::make_unique<MeshBuffer>(rect, rectIndices);
}

void AppLayer::onDetach() {
    player1.reset();
    player2.reset();
    m_Shader.reset();
}

void AppLayer::onUpdate(float deltaTime) {
    uint32_t width = m_app.getWindow().getWidth();
    uint32_t height = m_app.getWindow().getHeight();
    
    float aspectRatio = (height > 0) ? (static_cast<float>(width) / static_cast<float>(height)) : 1.0f;
    
    m_Projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
    m_View = glm::mat4(1.0f);
}

void AppLayer::onRender() {
    m_Shader->bind();
    
    uint32_t progID = m_Shader->getID();
    
    glUniformMatrix4fv(glGetUniformLocation(progID, "projection"), 1, GL_FALSE, glm::value_ptr(m_Projection));
    glUniformMatrix4fv(glGetUniformLocation(progID, "view"), 1, GL_FALSE, glm::value_ptr(m_View));

    glm::mat4 model1 = glm::translate(glm::mat4(1.0f), p1_position);
    glUniformMatrix4fv(glGetUniformLocation(progID, "model"), 1, GL_FALSE, glm::value_ptr(model1));
    player1->draw();

    glm::mat4 model2 = glm::translate(glm::mat4(1.0f), p2_position);
    glUniformMatrix4fv(glGetUniformLocation(progID, "model"), 1, GL_FALSE, glm::value_ptr(model2));
    player2->draw();
}
