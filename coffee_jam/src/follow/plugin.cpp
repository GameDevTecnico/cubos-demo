#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Follow)
{
    return cubos::core::ecs::TypeBuilder<Follow>("demo::Follow")
        .withField("distance", &Follow::distance)
        .withField("phi", &Follow::phi)
        .withField("theta", &Follow::theta)
        .withField("speed", &Follow::speed)
        .tree() // Many-to-one relation.
        .build();
}

void demo::followPlugin(Cubos& cubos)
{
    cubos.relation<Follow>();

    cubos.depends(transformPlugin);

    cubos.system("update Follow relation transforms")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Query<Position&, Rotation&, Follow&, const Position&> query) {
            for (auto [position, rotation, follow, targetPosition] : query)
            {
                // Calculate the position we want to be in.
                auto phiRad = glm::radians(follow.phi);
                auto thetaRad = glm::radians(follow.theta);
                glm::vec3 offset = glm::vec3(follow.distance * glm::cos(phiRad) * glm::cos(thetaRad),
                                             follow.distance * glm::cos(phiRad) * glm::sin(thetaRad),
                                             follow.distance * glm::sin(phiRad));
                glm::vec3 desired = targetPosition.vec + offset;

                // Move towards the desired position.
                glm::vec3 moveDirection = desired - position.vec;
                auto distanceToDesired = glm::length(moveDirection);

                if (follow.initialized)
                {
                    position.vec += moveDirection * glm::min(1.0F, follow.speed * dt.value() / distanceToDesired);
                }
                else
                {
                    position.vec = desired;
                    follow.initialized = true;
                }

                // Set the rotation to look at where the target would be if we had reached the desired position.
                rotation.quat = glm::quatLookAt(glm::normalize(-offset), glm::vec3(0.0F, 1.0F, 0.0F));
            }
        });
}
