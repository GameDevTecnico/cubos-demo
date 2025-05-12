#include "plugin.hpp"
#include "../interpolation/plugin.hpp"
#include "../car/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(roll_racers::Follow)
{
    return cubos::core::ecs::TypeBuilder<Follow>("roll_racers::Follow")
        .withField("distance", &Follow::distance)
        .withField("height", &Follow::height)
        .withField("angle", &Follow::angle)
        .withField("halfTime", &Follow::halfTime)
        .withField("rotationHalfTime", &Follow::rotationHalfTime)
        .tree() // Many-to-one relation.
        .build();
}

void roll_racers::followPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(carPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(interpolationPlugin);

    cubos.relation<Follow>();

    cubos.system("update Follow relation transforms")
        .before(transformUpdateTag)
        .with<Position>()
        .with<Rotation>()
        .with<PlayerCameraOwner>()
        .related<Follow>(0, 1)
        .related<InterpolationOf>(2, 1)
        .with<Position>(2)
        .with<Rotation>(2)
        .call(
            [](const DeltaTime& dt, Input& input,
               Query<Position&, Rotation&, const PlayerCameraOwner&, Follow&, const Position&, const Rotation&> query) {
                for (auto [position, rotation, owner, follow, targetPosition, targetRotation] : query)
                {
                    follow.angle += input.axis("look", owner.player) * 90.0F * dt.value();

                    // Direction "backward" from the target's rotation.
                    glm::vec3 backDir = glm::quat({0.0F, glm::radians(follow.angle), 0.0F}) * targetRotation.quat *
                                        glm::vec3(0.0F, 0.0F, 1.0F);
                    glm::vec3 upDir = glm::vec3(0.0F, 1.0F, 0.0F);

                    glm::vec3 desired = targetPosition.vec - backDir * follow.distance + upDir * follow.height;

                    // Move towards the desired position.
                    if (follow.initialized)
                    {
                        // Interpolate the right amount to reach half of the distance in halfTime.
                        auto alpha = 1.0F - glm::pow(0.5F, dt.value() / follow.halfTime);
                        position.vec = glm::mix(position.vec, desired, alpha);
                    }
                    else
                    {
                        position.vec = desired;
                    }

                    // Set the rotation to look at where the target would be if we had reached the desired position.
                    float alphaPos = 1.0F - glm::pow(0.5F, dt.value() / follow.halfTime);
                    position.vec = glm::mix(position.vec, desired, alphaPos);

                    // Calculate direction towards the target
                    glm::vec3 direction = glm::normalize(targetPosition.vec - position.vec);
                    glm::quat desiredRot = glm::quatLookAt(direction, upDir);

                    if (follow.initialized)
                    {
                        // Interpolate rotation separately
                        float alphaRot = 1.0F - glm::pow(0.5F, dt.value() / follow.rotationHalfTime);
                        rotation.quat = glm::slerp(rotation.quat, desiredRot, alphaRot);
                    }
                    else
                    {
                        rotation.quat = desiredRot;
                    }

                    follow.initialized = true;
                }
            });
}