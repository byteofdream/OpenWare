#pragma once

// Core transform module: position, rotation and scale composition for scene entities.

#include "Engine/Core/Math.hpp"

namespace ow {

struct Transform {
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 rotationEuler{0.0f, 0.0f, 0.0f};
    Vec3 scale{1.0f, 1.0f, 1.0f};

    Mat4 Matrix() const {
        const Mat4 t = Mat4::Translation(position);
        const Mat4 rx = Mat4::RotationX(rotationEuler.x);
        const Mat4 ry = Mat4::RotationY(rotationEuler.y);
        const Mat4 rz = Mat4::RotationZ(rotationEuler.z);
        const Mat4 s = Mat4::Scale(scale);
        return t * rz * ry * rx * s;
    }
};

} // namespace ow
