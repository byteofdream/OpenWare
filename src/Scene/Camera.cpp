#include "Engine/Scene/Camera.hpp"

namespace ow {

Vec3 Camera::Front() const {
    const float yawRad = Radians(yaw);
    const float pitchRad = Radians(pitch);
    Vec3 front{};
    front.x = std::cos(yawRad) * std::cos(pitchRad);
    front.y = std::sin(pitchRad);
    front.z = std::sin(yawRad) * std::cos(pitchRad);
    return Normalize(front);
}

Vec3 Camera::Right() const {
    return Normalize(Cross(Front(), Vec3{0.0f, 1.0f, 0.0f}));
}

Vec3 Camera::Up() const {
    return Normalize(Cross(Right(), Front()));
}

Mat4 Camera::ViewMatrix() const {
    return Mat4::LookAt(position, position + Front(), Up());
}

Mat4 Camera::ProjectionMatrix(float aspect) const {
    return Mat4::Perspective(Radians(fov), aspect, 0.1f, 100.0f);
}

void Camera::ProcessMouse(float deltaX, float deltaY, float sensitivity) {
    yaw += deltaX * sensitivity;
    pitch += deltaY * sensitivity;

    if (pitch > 89.0f) {
        pitch = 89.0f;
    }
    if (pitch < -89.0f) {
        pitch = -89.0f;
    }
}

} // namespace ow
