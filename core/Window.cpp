#include "Window.h"

Window::Window(const std::string& title, uint32_t width, uint32_t height) {
    m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!m_Window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    glfwMakeContextCurrent(m_Window);
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
}

uint32_t Window::getWidth() const {
    int w, h;
    glfwGetFramebufferSize(m_Window, &w, &h);
    return (uint32_t)w;
}

uint32_t Window::getHeight() const {
    int w, h;
    glfwGetFramebufferSize(m_Window, &w, &h);
    return (uint32_t)h;
}