#pragma once

#include "headers.h"

class Layer {
public:
    Layer(const std::string& name = "Layer") : m_DebugName(name) {}
    virtual ~Layer() = default;

    virtual void onAttach() {}
    virtual void onDetach() {}
    virtual void onUpdate(float deltaTime) {}
    virtual void onRender() {}

    const std::string& getName() const { return m_DebugName; }

protected:
    std::string m_DebugName;
};
