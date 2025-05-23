#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <glm/vec3.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/core/ecs/entity/entity.hpp>

namespace roll_racers
{
    /// @brief Component which represents a car drivable by a player
    struct GameRoundSettings
    {
        CUBOS_REFLECT;

        cubos::engine::Entity roundManagerEntity;

        int currentRound = 0;
        int maxRound = 3;

        float currentTimeBetweenRounds = 0.0F;
        float maxTimeBetweenRounds = 7.0F;
    };

    struct RoundManager
    {
        CUBOS_REFLECT;
    };

    struct Destroy
    {
        CUBOS_REFLECT;
    };

    struct Build
    {
        CUBOS_REFLECT;
    };

    struct BuildEnd
    {
        CUBOS_REFLECT;
    };

    struct RoundPlaying
    {
        CUBOS_REFLECT;
    };

    struct WaitingRoundStart
    {
        CUBOS_REFLECT;
        float time = 0.0F;
    };

    struct ShowEndScreen
    {
        CUBOS_REFLECT;
        float time = 0.0F;
        float maxTime = 5.0F;
    };

    struct EndArea
    {
        CUBOS_REFLECT;
    };

    /*
    // destroy everything with the delete thing
    struct BelondsToRound
    {
        CUBOS_REFLECT;
        int round = 0;
    };
    */

    /*
    struct Points
    {
        CUBOS_REFLECT;

        int points;
        // rewards per grabbing toilet paper
        // rewards per reaching end
    };
    */

    void roundManagerPlugin(cubos::engine::Cubos& cubos);
} // namespace roll_racers

// have a way to reset players
