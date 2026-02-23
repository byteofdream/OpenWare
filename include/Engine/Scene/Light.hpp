#pragma once

// Scene lighting module: minimal directional light for retro low-poly shading.

#include "Engine/Core/Math.hpp"

namespace ow {

struct DirectionalLight {
    Vec3 direction{-0.5f, -1.0f, -0.3f};
    Vec3 color{1.0f, 0.95f, 0.85f};
};

} // namespace ow
