#include "Application.h"

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        for (Layer* layer : app->getLayers()) {
            layer->onUpdate(0.0f);
        }
        for (Layer* layer : app->getLayers()) {
            layer->onRender();
        }
        glfwSwapBuffers(window);
    }
}

static void window_refresh_callback(GLFWwindow* window) {
    auto* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
    if (app) {
        for (Layer* layer : app->getLayers()) {
            layer->onUpdate(0.0f);
        }
        for (Layer* layer : app->getLayers()) {
            layer->onRender();
        }
        glfwSwapBuffers(window);
    }
}

Application::Application(const ApplicationSpecification& appSpec) {
    if (!glfwInit()) {
        throw std::runtime_error(glfwErrorGet().glfwErrorMsg("Failed to initialize GLFW!"));
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

    m_window = std::make_unique<Window>(
        appSpec.title,
        appSpec.width,
        appSpec.height
    );

    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to load OpenGL functions from glad!");
    }

    glfwSetWindowUserPointer(m_window->getWindow(), this);
    glfwSetFramebufferSizeCallback(m_window->getWindow(), framebuffer_size_callback);
    glfwSetWindowRefreshCallback(m_window->getWindow(), window_refresh_callback);

    m_LastFrameTime = std::chrono::steady_clock::now();
}

Application::~Application() {
    for (Layer* layer : m_LayerStack) {
        layer->onDetach();
    }
    glfwTerminate();
}

void Application::pushLayer(Layer* layer) {
    m_LayerStack.push_back(layer);
    layer->onAttach();
}

void Application::run() {
    while (!glfwWindowShouldClose(m_window->getWindow())) {
        auto currentFrameTime = std::chrono::steady_clock::now();
        float deltaTime = std::chrono::duration<float>(currentFrameTime - m_LastFrameTime).count();
        m_LastFrameTime = currentFrameTime;

        glfwPollEvents();

        if (glfwGetKey(m_window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(m_window->getWindow(), true);
        }

        for (Layer* layer : m_LayerStack) {
            layer->onUpdate(deltaTime);
        }

        glClearColor(0.1f, 0.1f, 0.2f, 0.5f);
        glClear(GL_COLOR_BUFFER_BIT);

        for (Layer* layer : m_LayerStack) {
            layer->onRender();
        }

        glfwSwapBuffers(m_window->getWindow());
    }

    for (Layer* layer : m_LayerStack) {
        layer->onDetach();
    }
    m_LayerStack.clear();
}