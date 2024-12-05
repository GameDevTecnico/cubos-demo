#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Relation which makes an entity smoothly orbit another entity.
    struct Follow
    {
        CUBOS_REFLECT;

        float distance{1.0F};
        float phi{0.0F};
        float theta{0.0F};

        /// @brief Time in seconds until the camera reaches half of the distance to the target.
        float halfTime{1.0F};

        /// @brief Optional entity to base the yaw and position on.
        cubos::engine::Entity basedOn{};

        /// @brief Height offset from the target entity.
        float height{0.0F};

        /// @brief Point the entity is actually following, which follows the target entity.
        glm::vec3 center{0.0F};

        bool initialized{false};
    };

    void followPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
