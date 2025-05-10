#pragma once

#include <unordered_map>

#include <cubos/core/ecs/entity/entity.hpp>
#include <cubos/core/ecs/entity/hash.hpp>
#include <cubos/core/reflection/reflect.hpp>

#include <cubos/engine/prelude.hpp>

namespace coffee
{
    /// @brief Component which makes a UI element blink
    struct UIBlink
    {
        CUBOS_REFLECT;
    };

    /// @brief Component which manages score UIs for each player viewport in the render target
    struct ScoreUIManager
    {
        CUBOS_REFLECT;

        std::unordered_map<cubos::core::ecs::Entity, cubos::core::ecs::Entity, cubos::core::ecs::EntityHash>
            scoreUIs; ///< Maps camera to score UI.
    };

    /// @brief Component which marks a UI as being a score display
    struct ScoreUI
    {
        CUBOS_REFLECT;

        int player = 1;
    };

    /// @brief Component which marks the player name in the scoreboard
    struct ScoreboardPlayerName
    {
        CUBOS_REFLECT;

        int order = 1;
    };

    /// @brief Component which marks the player score in the scoreboard
    struct ScoreboardPlayerScore
    {
        CUBOS_REFLECT;

        int order = 1;
    };

    void uiEffectsPlugin(cubos::engine::Cubos& cubos);
} // namespace coffee
