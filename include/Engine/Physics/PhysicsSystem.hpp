#pragma once

// Physics module: basic rigid body simulation with sphere collision response.

namespace ow {

class Entity;
class Scene;

class PhysicsSystem {
public:
    static void Step(Scene& scene, float deltaTime, int substeps = 4);
    static bool CheckSphereCollision(const Entity& a, const Entity& b);
};

} // namespace ow
