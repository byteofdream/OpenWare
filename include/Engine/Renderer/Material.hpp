#pragma once

// Renderer material module: shader binding plus albedo/emissive texture slots.

#include <memory>

#include "Engine/Core/Math.hpp"

namespace ow {

class Shader;

class Material {
public:
    std::shared_ptr<Shader> shader;

    // Base surface response.
    Vec3 color{0.8f, 0.8f, 0.8f};
    float roughness = 0.8f;

    // Emissive parameters for simple glow-like stylization.
    Vec3 emissiveColor{0.0f, 0.0f, 0.0f};
    float emissiveStrength = 0.0f;

    // Texture slots.
    unsigned int albedoTextureId = 0;
    unsigned int emissiveTextureId = 0;
    bool useAlbedoTexture = false;
    bool useEmissiveTexture = false;
};

} // namespace ow
