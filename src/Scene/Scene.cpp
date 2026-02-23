#include "Engine/Scene/Scene.hpp"

#include "Engine/Scene/Entity.hpp"

namespace ow {

std::shared_ptr<Entity> Scene::AddEntity(const std::shared_ptr<Entity>& entity) {
    entities.push_back(entity);
    return entity;
}

} // namespace ow
