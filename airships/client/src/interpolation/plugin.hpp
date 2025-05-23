#pragma once

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace airships::client
{
    /// @brief Relation which sets the local transform of the source entity to the interpolated local transforms of the
    /// target entity.
    ///
    /// This is useful for when an entity is physically simulated or updated at a lower frame rate than the rendering
    /// frame rate. With this relation, it is possible to split the simulation and rendering into two separate entities,
    /// in order to guarantee a smooth animation.
    struct InterpolationOf
    {
        CUBOS_REFLECT;

        glm::vec3 previousPosition{};
        glm::quat previousRotation{};
        float previousScale{0.0F};
        glm::vec3 nextPosition{};
        glm::quat nextRotation{};
        float nextScale{0.0F};
    };

    /// @brief Component which spawns a new scene whose "root" entity is interpolated from the current entity.
    ///
    /// Any children entities with the same component will put their interpolated entities as children of the
    /// interpolated counterpart of this entity.
    struct Interpolated
    {
        CUBOS_REFLECT;

        cubos::engine::Asset<cubos::engine::Scene> scene;
    };

    void interpolationPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client