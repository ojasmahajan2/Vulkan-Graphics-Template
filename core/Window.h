#pragma once

#include "headers.h"

class Window {
public:
    Window(const std::string& title, uint32_t width, uint32_t height);
    ~Window();

    GLFWwindow* getWindow() const { return m_Window; }
    
    uint32_t getWidth() const;
    uint32_t getHeight() const;

private:
    GLFWwindow* m_Window;
};