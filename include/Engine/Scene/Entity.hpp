#pragma once

// Scene entity module: scene node that combines transform, mesh, material and collider radius.

#include <memory>
#include <string>

#include "Engine/Core/Transform.hpp"

namespace ow {

class Mesh;
class Material;
struct Rigidbody;

class Entity {
public:
    explicit Entity(std::string entityName);

    std::string name;
    Transform transform;
    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Material> material;
    std::shared_ptr<Rigidbody> rigidbody;

    // Physics collider radius used by simple sphere collision checks.
    float colliderRadius = 0.5f;
};

} // namespace ow
