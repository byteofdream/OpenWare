#include "Engine/Scene/Entity.hpp"

#include <utility>

namespace ow {

Entity::Entity(std::string entityName) : name(std::move(entityName)) {}

} // namespace ow
