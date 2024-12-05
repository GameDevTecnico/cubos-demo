#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Follow)
{
    return cubos::core::ecs::TypeBuilder<Follow>("airships::client::Follow")
        .withField("distance", &Follow::distance)
        .withField("phi", &Follow::phi)
        .withField("theta", &Follow::theta)
        .withField("halfTime", &Follow::halfTime)
        .withField("considerRotation", &Follow::considerRotation)
        .withField("center", &Follow::center)
        .tree() // Many-to-one relation.
        .build();
}

void airships::client::followPlugin(Cubos& cubos)
{
    cubos.relation<Follow>();

    cubos.depends(transformPlugin);

    cubos.system("update Follow relation transforms")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Query<Position&, Rotation&, Follow&, const LocalToWorld&> query) {
            for (auto [position, rotation, follow, targetLTW] : query)
            {
                auto targetPosition = targetLTW.worldPosition();
                auto targetRotation = targetLTW.worldRotation();

                // Interpolate the center to the current target position.
                if (follow.initialized)
                {
                    auto alpha = 1.0F - glm::pow(0.5F, dt.value() / follow.halfTime);
                    follow.center = glm::mix(follow.center, targetPosition, alpha);
                }
                else
                {
                    follow.center = targetPosition;
                    follow.initialized = true;
                }

                // Calculate the position we want to be in.
                auto phiRad = glm::radians(follow.phi);
                auto thetaRad = glm::radians(follow.theta);
                if (follow.considerRotation)
                {
                    auto targetRotationMat = glm::mat3_cast(targetRotation);

                    // Extract yaw from the target's rotation.
                    auto targetYaw = glm::atan(targetRotationMat[2][0], targetRotationMat[2][2]);
                    thetaRad -= targetYaw;
                }
                glm::vec3 offset = glm::vec3(follow.distance * glm::cos(phiRad) * glm::cos(thetaRad),
                                             follow.distance * glm::sin(phiRad),
                                             follow.distance * glm::cos(phiRad) * glm::sin(thetaRad));
                position.vec = follow.center + offset;

                // Set the rotation to look at where the target would be if we had reached the desired position.
                rotation.quat = glm::quatLookAt(glm::normalize(-offset), glm::vec3(0.0F, 1.0F, 0.0F));
            }
        });
}
