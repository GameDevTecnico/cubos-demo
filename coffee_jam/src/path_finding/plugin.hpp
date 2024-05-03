#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/thread/task.hpp>

#include <cubos/engine/prelude.hpp>

#include <glm/vec2.hpp>

namespace demo
{
    /// @brief Component which when added, spawns a path-finding thread.
    ///
    /// When finished, a Path component is added to the entity.
    struct PathTask
    {
        CUBOS_REFLECT;

        /// @brief Position to start at.
        glm::ivec2 from;

        /// @brief Team of the entity which will traverse the path.
        int team{0};

        /// @brief Penalization for each HP point in the path.
        float hpPenalization{1.0F};

        /// @brief Task to find the path.
        cubos::core::thread::Task<std::vector<glm::ivec2>> task{};
    };

    /// @brief Component for targets for path-finding.
    struct Target
    {
        CUBOS_REFLECT;

        /// @brief Target cost for the path-finding, lower is preferred.
        float cost{0.0F};
    };

    /// @brief Component added when a path-finding task is finished.
    struct Path
    {
        CUBOS_REFLECT;

        /// @brief Path from the end to the start (reverse order).
        std::vector<glm::ivec2> path;
    };

    void pathFindingPlugin(cubos::engine::Cubos& cubos);
} // namespace demo
