#pragma once

#include <glm/vec2.hpp>

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace demo
{
    /// @brief Relation linking sailor entities and their player entities.
    ///
    /// Animates the sailor's death.
    struct SailorDeath
    {
        CUBOS_REFLECT;

        /// @brief HP level at which the sailor dies.
        int hp = 0;
    };

    /// @brief Holds sailor death animation data.
    struct SailorDeathAnimation
    {
        CUBOS_REFLECT;

        /// @brief Progress where 0 is the source and 1 is the target.
        float time{0.0F};

        /// @brief Speed of the animation.
        float speed{1.0F};

        /// @brief Duration of the animation.
        float duration{5.0F};

        /// @brief Source position.
        glm::vec2 source{};

        /// @brief Target position.
        glm::vec2 target{};

        /// @brief Initial trajectory height.
        float initialHeight{0.0F};

        /// @brief Maximum trajectory height.
        float maxHeight{1.0F};
    };

    void sailorDeathPlugin(cubos::engine::Cubos& cubos);
} // namespace demo