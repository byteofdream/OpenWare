#pragma once

// Physics module: rigid body data used by the physics solver.

#include "Engine/Core/Math.hpp"

namespace ow {

struct Rigidbody {
    Vec3 velocity{0.0f, 0.0f, 0.0f};
    Vec3 accumulatedForce{0.0f, 0.0f, 0.0f};

    float mass = 1.0f;
    float inverseMass = 1.0f;
    float linearDamping = 0.98f;
    float restitution = 0.25f;

    bool useGravity = true;
    bool isStatic = false;

    void SetMass(float newMass) {
        if (newMass <= 0.00001f) {
            mass = 1.0f;
            inverseMass = 1.0f;
            return;
        }
        mass = newMass;
        inverseMass = 1.0f / mass;
    }

    void AddForce(const Vec3& force) {
        accumulatedForce += force;
    }

    void AddImpulse(const Vec3& impulse) {
        if (isStatic) {
            return;
        }
        velocity += impulse * inverseMass;
    }
};

} // namespace ow
