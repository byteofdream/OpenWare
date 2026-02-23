#include "Engine/Physics/PhysicsSystem.hpp"

#include <algorithm>
#include <vector>

#include "Engine/Core/Math.hpp"
#include "Engine/Physics/Rigidbody.hpp"
#include "Engine/Scene/Entity.hpp"
#include "Engine/Scene/Scene.hpp"

namespace ow {

namespace {

const Vec3 kGravity{0.0f, -9.81f, 0.0f};

float InverseMass(const Entity& entity) {
    if (!entity.rigidbody || entity.rigidbody->isStatic) {
        return 0.0f;
    }
    return entity.rigidbody->inverseMass;
}

void Integrate(Entity& entity, float dt) {
    if (!entity.rigidbody || entity.rigidbody->isStatic) {
        return;
    }

    auto& rb = *entity.rigidbody;
    Vec3 totalForce = rb.accumulatedForce;
    if (rb.useGravity) {
        totalForce += kGravity * rb.mass;
    }

    const Vec3 acceleration = totalForce * rb.inverseMass;
    rb.velocity += acceleration * dt;

    const float damping = std::clamp(rb.linearDamping, 0.0f, 1.0f);
    rb.velocity = rb.velocity * damping;
    entity.transform.position += rb.velocity * dt;

    rb.accumulatedForce = Vec3{0.0f, 0.0f, 0.0f};
}

void ResolvePair(Entity& a, Entity& b) {
    const Vec3 delta = b.transform.position - a.transform.position;
    float distance = Length(delta);
    const float minDistance = a.colliderRadius + b.colliderRadius;
    if (distance >= minDistance || minDistance <= 0.0f) {
        return;
    }

    Vec3 normal = Vec3{1.0f, 0.0f, 0.0f};
    if (distance > 0.00001f) {
        normal = delta / distance;
    } else {
        distance = 0.0f;
    }

    const float invMassA = InverseMass(a);
    const float invMassB = InverseMass(b);
    const float invMassSum = invMassA + invMassB;
    if (invMassSum <= 0.0f) {
        return;
    }

    // Positional correction to remove overlap and reduce sinking.
    const float penetration = minDistance - distance;
    const float correctionPercent = 0.8f;
    const Vec3 correction = normal * (penetration * correctionPercent / invMassSum);
    a.transform.position += correction * -invMassA;
    b.transform.position += correction * invMassB;

    Vec3 velocityA{0.0f, 0.0f, 0.0f};
    Vec3 velocityB{0.0f, 0.0f, 0.0f};
    if (a.rigidbody) {
        velocityA = a.rigidbody->velocity;
    }
    if (b.rigidbody) {
        velocityB = b.rigidbody->velocity;
    }

    const Vec3 relativeVelocity = velocityB - velocityA;
    const float velocityAlongNormal = Dot(relativeVelocity, normal);
    if (velocityAlongNormal > 0.0f) {
        return;
    }

    float restitution = 0.2f;
    if (a.rigidbody && b.rigidbody) {
        restitution = std::min(a.rigidbody->restitution, b.rigidbody->restitution);
    } else if (a.rigidbody) {
        restitution = a.rigidbody->restitution;
    } else if (b.rigidbody) {
        restitution = b.rigidbody->restitution;
    }

    const float j = -(1.0f + restitution) * velocityAlongNormal / invMassSum;
    const Vec3 impulse = normal * j;

    if (a.rigidbody && !a.rigidbody->isStatic) {
        a.rigidbody->velocity += impulse * -invMassA;
    }
    if (b.rigidbody && !b.rigidbody->isStatic) {
        b.rigidbody->velocity += impulse * invMassB;
    }
}

} // namespace

void PhysicsSystem::Step(Scene& scene, float deltaTime, int substeps) {
    if (deltaTime <= 0.0f) {
        return;
    }

    const int iterations = std::max(1, substeps);
    const float dt = deltaTime / static_cast<float>(iterations);

    std::vector<Entity*> bodies;
    bodies.reserve(scene.entities.size());
    for (const auto& e : scene.entities) {
        if (e && e->colliderRadius > 0.0f) {
            bodies.push_back(e.get());
        }
    }

    for (int step = 0; step < iterations; ++step) {
        for (Entity* entity : bodies) {
            Integrate(*entity, dt);
        }

        for (std::size_t i = 0; i < bodies.size(); ++i) {
            for (std::size_t j = i + 1; j < bodies.size(); ++j) {
                ResolvePair(*bodies[i], *bodies[j]);
            }
        }
    }
}

bool PhysicsSystem::CheckSphereCollision(const Entity& a, const Entity& b) {
    const Vec3 delta = a.transform.position - b.transform.position;
    const float distance = Length(delta);
    return distance <= (a.colliderRadius + b.colliderRadius);
}

} // namespace ow
