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

        /// @brief Position to get to.
        glm::ivec2 to;

        /// @brief Task to find the path.
        cubos::core::thread::Task<std::vector<glm::ivec2>> task{};
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
