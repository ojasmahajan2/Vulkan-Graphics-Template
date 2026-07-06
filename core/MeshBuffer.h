#pragma once

#include "headers.h"

class MeshBuffer {
public:
    MeshBuffer(const std::vector<float>& vertices, const std::vector<uint32_t>& indices);
    ~MeshBuffer();

    void bind() const;
    void unbind() const;
    void draw() const;

private:
    uint32_t m_VAO;
    uint32_t m_VBO;
    uint32_t m_EBO;
    uint32_t m_IndexCount;
};