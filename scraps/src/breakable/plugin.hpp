#pragma once

#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/scene/scene.hpp>

namespace demo
{
    /// @brief Component which allows an object to be broken when interacted with.
    struct Breakable
    {
        CUBOS_REFLECT;

        /// @brief Object to spawn in the same position as the entity when broken.
        cubos::engine::Asset<cubos::engine::Scene> drop;

        /// @brief Name of the root entity of the drop scene.
        std::string root;

        /// @brief Minimum number of items dropped.
        int minDrops{1};

        /// @brief Maximum number of items dropped.
        int maxDrops{1};
    };

    void breakablePlugin(cubos::engine::Cubos& cubos);
} // namespace demo
