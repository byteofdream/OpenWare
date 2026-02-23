#include "Engine/Renderer/Mesh.hpp"

#include "Engine/Renderer/GL.hpp"

#include <cstddef>

#include <utility>

namespace ow {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    Upload(vertices, indices);
}

Mesh::~Mesh() {
    Release();
}

Mesh::Mesh(Mesh&& other) noexcept {
    vao_ = other.vao_;
    vbo_ = other.vbo_;
    ebo_ = other.ebo_;
    indexCount_ = other.indexCount_;

    other.vao_ = 0;
    other.vbo_ = 0;
    other.ebo_ = 0;
    other.indexCount_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        Release();
        vao_ = other.vao_;
        vbo_ = other.vbo_;
        ebo_ = other.ebo_;
        indexCount_ = other.indexCount_;

        other.vao_ = 0;
        other.vbo_ = 0;
        other.ebo_ = 0;
        other.indexCount_ = 0;
    }
    return *this;
}

void Mesh::Upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    indexCount_ = static_cast<int>(indices.size());

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindVertexArray(0);
}

void Mesh::Draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::Release() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_ != 0) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    indexCount_ = 0;
}

std::shared_ptr<Mesh> Mesh::CreateCube(float halfExtent) {
    const float h = halfExtent;
    const std::vector<Vertex> vertices = {
        {{-h, -h,  h}, {0, 0, 1}, {0, 0}}, {{ h, -h,  h}, {0, 0, 1}, {1, 0}}, {{ h,  h,  h}, {0, 0, 1}, {1, 1}}, {{-h,  h,  h}, {0, 0, 1}, {0, 1}},
        {{ h, -h, -h}, {0, 0,-1}, {0, 0}}, {{-h, -h, -h}, {0, 0,-1}, {1, 0}}, {{-h,  h, -h}, {0, 0,-1}, {1, 1}}, {{ h,  h, -h}, {0, 0,-1}, {0, 1}},
        {{-h, -h, -h}, {-1,0, 0}, {0, 0}}, {{-h, -h,  h}, {-1,0, 0}, {1, 0}}, {{-h,  h,  h}, {-1,0, 0}, {1, 1}}, {{-h,  h, -h}, {-1,0, 0}, {0, 1}},
        {{ h, -h,  h}, {1, 0, 0}, {0, 0}}, {{ h, -h, -h}, {1, 0, 0}, {1, 0}}, {{ h,  h, -h}, {1, 0, 0}, {1, 1}}, {{ h,  h,  h}, {1, 0, 0}, {0, 1}},
        {{-h,  h,  h}, {0, 1, 0}, {0, 0}}, {{ h,  h,  h}, {0, 1, 0}, {1, 0}}, {{ h,  h, -h}, {0, 1, 0}, {1, 1}}, {{-h,  h, -h}, {0, 1, 0}, {0, 1}},
        {{-h, -h, -h}, {0,-1, 0}, {0, 0}}, {{ h, -h, -h}, {0,-1, 0}, {1, 0}}, {{ h, -h,  h}, {0,-1, 0}, {1, 1}}, {{-h, -h,  h}, {0,-1, 0}, {0, 1}},
    };

    const std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0,
        4, 5, 6, 6, 7, 4,
        8, 9,10,10,11, 8,
       12,13,14,14,15,12,
       16,17,18,18,19,16,
       20,21,22,22,23,20,
    };

    return std::make_shared<Mesh>(vertices, indices);
}

} // namespace ow
