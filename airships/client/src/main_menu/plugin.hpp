#pragma once

#include "../level_generator/plugin.hpp"

#include <cubos/engine/prelude.hpp>

#include <string>
#include <vector>

namespace airships::client
{
    struct MainMenu
    {
        CUBOS_REFLECT;

        LevelGenerator levelGenerator;
        std::vector<cubos::engine::Asset<cubos::engine::Scene>> boats;
        std::vector<cubos::engine::Asset<cubos::engine::Scene>> characters;
    };

    struct OnlineMenu
    {
        CUBOS_REFLECT;

        std::string address{"127.0.0.1"};
        int port{22749};
        enum class Status
        {
            Disconnected,
            Connecting,
            Failed,
        } status = Status::Disconnected;

        MainMenu mainMenu;
    };

    struct OfflineMenu
    {
        CUBOS_REFLECT;

        struct Player
        {
            int id;
        };

        struct Team
        {
            std::vector<Player> players;
        };

        std::vector<Team> teams = {{.players = {{.id = 1}}}, {.players = {{.id = 2}}}};

        MainMenu mainMenu;
    };

    struct RestartMenu
    {
        CUBOS_REFLECT;

        MainMenu mainMenu;
    };

    void mainMenuPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
