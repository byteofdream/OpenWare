#pragma once

// Scene module: stores entities and world lighting settings.

#include <memory>
#include <vector>

#include "Engine/Scene/Light.hpp"

namespace ow {

class Entity;

class Scene {
public:
    std::vector<std::shared_ptr<Entity>> entities;
    DirectionalLight light;

    std::shared_ptr<Entity> AddEntity(const std::shared_ptr<Entity>& entity);
};

} // namespace ow
