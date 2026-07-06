#pragma once

#include "headers.h"

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void bind() const;
    void unbind() const;

    uint32_t getID() const { return m_RendererID; }

private:
    uint32_t m_RendererID;
    
    std::string extractGLSL(const std::string& path);
};
