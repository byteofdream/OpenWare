#pragma once

// Scene camera module: first-person camera movement and view/projection helpers.

#include "Engine/Core/Math.hpp"

namespace ow {

class Camera {
public:
    Vec3 position{0.0f, 1.5f, 6.0f};
    float yaw = -90.0f;
    float pitch = -10.0f;
    float fov = 60.0f;

    Mat4 ViewMatrix() const;
    Mat4 ProjectionMatrix(float aspect) const;

    Vec3 Front() const;
    Vec3 Right() const;
    Vec3 Up() const;

    void ProcessMouse(float deltaX, float deltaY, float sensitivity = 0.1f);
};

} // namespace ow
