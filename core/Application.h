#pragma once

#include "Window.h"
#include "Layer.h"

struct ApplicationSpecification {
    uint32_t width  = 854;
    uint32_t height = 480;

    const char* title = "OpenGL Graphics Application";
};

class Application {
public:
    Application(const ApplicationSpecification& appSpec);
    ~Application();

    void run();
    void pushLayer(Layer* layer);

    Window& getWindow() { return *m_window; }
    std::vector<Layer*>& getLayers() { return m_LayerStack; }

private:
    std::unique_ptr<Window> m_window;
    std::vector<Layer*> m_LayerStack;

    std::chrono::time_point<std::chrono::steady_clock> m_LastFrameTime;
};