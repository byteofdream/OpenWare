#pragma once

// Renderer module: central draw pass for scene entities.

#include "Engine/Core/Math.hpp"
#include "Engine/UI/DebugUI.hpp"

namespace ow {

class Scene;
class Camera;

class Renderer {
public:
    void Render(const Scene& scene, const Camera& camera, int width, int height, const RenderSettings& settings) const;
};

} // namespace ow
