#pragma once

// Renderer mesh module: stores vertex/index buffers and draws geometry.

#include <memory>
#include <vector>

#include "Engine/Core/Math.hpp"

namespace ow {

struct Vertex {
    Vec3 position;
    Vec3 normal;
    Vec2 uv;
};

class Mesh {
public:
    Mesh() = default;
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw() const;

    static std::shared_ptr<Mesh> CreateCube(float halfExtent = 0.5f);

private:
    void Upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Release();

    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;
    int indexCount_ = 0;
};

} // namespace ow
