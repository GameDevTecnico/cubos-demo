#include "plugin.hpp"

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>

using namespace cubos::engine;

static CUBOS_DEFINE_TAG(drivableControlTag);

CUBOS_REFLECT_IMPL(airships::client::Drivable)
{
    return cubos::core::ecs::TypeBuilder<Drivable>("airships::client::Drivable")
        .withField("topLinearVelocity", &Drivable::topLinearVelocity)
        .withField("linearVelocity", &Drivable::linearVelocity)
        .withField("linearAcceleration", &Drivable::linearAcceleration)
        .withField("topAngularVelocity", &Drivable::topAngularVelocity)
        .withField("angularVelocity", &Drivable::angularVelocity)
        .withField("targetAngularVelocity", &Drivable::targetAngularVelocity)
        .withField("angularAcceleration", &Drivable::angularAcceleration)
        .withField("maxRoll", &Drivable::maxRoll)
        .withField("roll", &Drivable::roll)
        .withField("rollLerpFactor", &Drivable::rollLerpFactor)
        .withField("yaw", &Drivable::yaw)
        .withField("buoyancy", &Drivable::buoyancy)
        .build();
}

struct CollisionMovementModifiers
{
    CUBOS_ANONYMOUS_REFLECT(CollisionMovementModifiers);

    float time = 0.0F;
    float torque = 0.0F;
};

void airships::client::drivablePlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(gravityPlugin);

    cubos.component<Drivable>();
    cubos.resource<CollisionMovementModifiers>();

    cubos.tag(drivableControlTag);

    auto initBoatYaw = [](Query<Drivable&, const Position&> query) {
        for (auto [drivable, position] : query)
        {
            // Initialize boat so that it points to the center of the map
            drivable.yaw = glm::degrees(glm::atan(-position.vec.x, -position.vec.z));
        }
    };

    cubos.observer("initialize boat yaw").onAdd<Drivable>().call(initBoatYaw);
    cubos.observer("initialize boat yaw").onAdd<Position>().call(initBoatYaw);

    cubos.system("set modifiers on collision")
        .tagged(physicsApplyForcesTag)
        .call([](Query<Entity, Drivable&, Position&, Rotation&, Velocity&, Impulse&, AngularVelocity&, CollidingWith&,
                       Entity, Position&>
                     query,
                 CollisionMovementModifiers& collisionModifiers) {
            for (auto [ent1, drivable, pos1, rotation, velocity, impulse, angVelocity, collidingWith, ent2, pos2] :
                 query)
            {
                auto forward = glm::normalize(rotation.quat * glm::vec3{0.0F, 0.0F, 1.0F});
                auto collisionDirection = glm::normalize(pos2.vec - pos1.vec);
                auto dot = glm::dot(forward, collisionDirection);

                if (dot > -0.45F && dot < 0.45F)
                {
                    glm::vec3 right = {1.0F, 0.0F, 0.0F};
                    auto turnDirection = glm::dot(collisionDirection, right) > 0.0F ? -1.0F : 1.0F;
                    collisionModifiers.torque = glm::abs(dot) * turnDirection * drivable.topAngularVelocity * 2.0F;
                }
                else
                {
                    collisionModifiers.torque = 0.0F;
                }

                collisionModifiers.time = 1.0F;
            }
        });

    cubos.system("control Drivable entities")
        .tagged(drivableControlTag)
        .before(transformUpdateTag)
        .call([](Input& input, Query<Drivable&, Position&, Rotation&> drivables, const DeltaTime& dt,
                 CollisionMovementModifiers& collisionModifiers) {
            for (auto [drivable, position, rotation] : drivables)
            {
                //  Update angular velocity
                auto missingAngularVelocity = drivable.targetAngularVelocity - drivable.angularVelocity;
                auto availableAngularVelocity = drivable.angularAcceleration * dt.value();
                drivable.angularVelocity +=
                    glm::clamp(missingAngularVelocity, -availableAngularVelocity, availableAngularVelocity);

                auto topAngularVelocity =
                    (glm::abs(drivable.linearVelocity) / drivable.topLinearVelocity) * drivable.topAngularVelocity;

                drivable.angularVelocity =
                    glm::clamp(drivable.angularVelocity, -topAngularVelocity, topAngularVelocity);
                if (collisionModifiers.time >= 0.0F)
                {
                    drivable.angularVelocity += collisionModifiers.torque * dt.value();
                }

                // Apply angular velocity and compute rotation
                drivable.yaw -= drivable.angularVelocity * dt.value();
                drivable.yaw = glm::mod(drivable.yaw, 360.0F);
                rotation.quat = glm::angleAxis(glm::radians(drivable.yaw), glm::vec3{0.0F, 1.0F, 0.0F});

                auto rollFactor = drivable.angularVelocity / drivable.topAngularVelocity;
                auto targetRoll = rollFactor * drivable.maxRoll;
                drivable.roll =
                    glm::mix(drivable.roll, targetRoll, 1 - glm::pow(1.0 - drivable.rollLerpFactor, dt.value()));
                rotation.quat *= glm::angleAxis(glm::radians(drivable.roll), glm::vec3{0.0F, 0.0F, 1.0F});
            }
        });

    cubos.system("auto move Drivable entities")
        .tagged(physicsApplyForcesTag)
        .after(drivableControlTag)
        .call([](Input& input, const Gravity& gravity,
                 Query<Drivable&, Position&, Rotation&, Velocity&, const Mass&, Force&> drivables,
                 const FixedDeltaTime& dt, CollisionMovementModifiers& collisionModifiers) {
            for (auto [drivable, position, rotation, velocity, mass, force] : drivables)
            {
                // Update velocity position
                auto forward = rotation.quat * glm::vec3{0.0F, 0.0F, 1.0F};

                // Don't push the airship immediately forward and slow it down
                if (collisionModifiers.time >= 0.0F)
                {
                    collisionModifiers.time -= dt.value;
                    velocity.vec *= 0.97F;
                    continue;
                }

                glm::vec3 vel = velocity.vec;
                vel.y = 0.0F;
                float sign = glm::dot(vel, forward) < 0.0F ? -1 : 1;
                vel = forward * ((glm::length(vel) * sign) + (drivable.linearAcceleration * dt.value));

                vel = glm::clamp(vel, -drivable.topLinearVelocity, drivable.topLinearVelocity);

                // Update linear velocity
                drivable.linearVelocity = glm::length(vel);

                velocity.vec.x = vel.x;
                velocity.vec.z = vel.z;

                glm::vec3 buoyancy = -mass.mass * gravity.value * drivable.buoyancy;
                if (position.vec.y < 0.0F && drivable.buoyancy > 0.0F && velocity.vec.y < 0.0F)
                {
                    // Negate vertical velocity
                    buoyancy += glm::vec3{0.0F, -velocity.vec.y * mass.mass, 0.0F};
                }
                force.add(buoyancy);
            }
        });
}