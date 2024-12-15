#include "plugin.hpp"
#include "../team_spawner/plugin.hpp"
#include "../destroy_tree/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/imgui/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <imgui_stdlib.h>

#include <glm/common.hpp>

#include <unordered_set>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::MainMenu)
{
    return cubos::core::ecs::TypeBuilder<MainMenu>("airships::client::MainMenu")
        .withField("levelGenerator", &MainMenu::levelGenerator)
        .withField("boats", &MainMenu::boats)
        .withField("characters", &MainMenu::characters)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::OnlineMenu)
{
    return cubos::core::ecs::TypeBuilder<OnlineMenu>("airships::client::OnlineMenu")
        .withField("address", &OnlineMenu::address)
        .withField("port", &OnlineMenu::port)
        .withField("mainMenu", &OnlineMenu::mainMenu)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::OfflineMenu)
{
    return cubos::core::ecs::TypeBuilder<OfflineMenu>("airships::client::OfflineMenu")
        .withField("mainMenu", &OfflineMenu::mainMenu)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::RestartMenu)
{
    return cubos::core::ecs::TypeBuilder<RestartMenu>("airships::client::RestartMenu")
        .withField("mainMenu", &RestartMenu::mainMenu)
        .build();
}

void airships::client::mainMenuPlugin(Cubos& cubos)
{
    cubos.depends(imguiPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(levelGeneratorPlugin);
    cubos.depends(teamSpawnerPlugin);
    cubos.depends(destroyTreePlugin);

    cubos.component<MainMenu>();
    cubos.component<OnlineMenu>();
    cubos.component<OfflineMenu>();
    cubos.component<RestartMenu>();

    cubos.system("show main menu")
        .tagged(imguiTag)
        .call([](Commands cmds, Query<Entity, MainMenu&> query, ShouldQuit& shouldQuit) {
            for (auto [ent, state] : query)
            {
                // Center ImGui window
                ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2),
                                        ImGuiCond_Always, ImVec2(0.5F, 0.5F));
                if (!ImGui::Begin("Main Menu", nullptr,
                                  ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                      ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::End();
                    continue;
                }

                ImGui::NewLine();
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Welcome to").x) / 2);
                ImGui::Text("Welcome to");
                ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize("Airships").x) / 2);
                ImGui::Text("Airships!");
                ImGui::NewLine();

                if (ImGui::Button("Play Offline", ImVec2(200, 0)))
                {
                    cmds.remove<MainMenu>(ent);
                    cmds.add(ent, OfflineMenu{.mainMenu = state});
                }
                if (ImGui::Button("Play Online", ImVec2(200, 0)))
                {
                    cmds.remove<MainMenu>(ent);
                    cmds.add(ent, OnlineMenu{.mainMenu = state});
                }
                if (ImGui::Button("Quit", ImVec2(200, 0)))
                {
                    shouldQuit.value = true;
                }

                ImGui::End();
            }
        });

    cubos.system("show online menu").tagged(imguiTag).call([](Commands cmds, Query<Entity, OnlineMenu&> query) {
        for (auto [ent, state] : query)
        {
            // Center ImGui window
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2),
                                    ImGuiCond_Always, ImVec2(0.5F, 0.5F));
            if (!ImGui::Begin("Online Play", nullptr,
                              ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::End();
                continue;
            }

            if (state.status == OnlineMenu::Status::Connecting)
            {
                ImGui::Text("Connecting to %s:%d...", state.address.c_str(), state.port);

                if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                {
                    state.status = OnlineMenu::Status::Disconnected;
                }
            }
            else
            {
                ImGui::InputText("Address", &state.address);
                ImGui::InputInt("Port", &state.port);

                if (state.status == OnlineMenu::Status::Failed)
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "Failed to connect");
                }

                if (ImGui::Button("Connect", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                {
                    state.status = OnlineMenu::Status::Connecting;
                }
                if (ImGui::Button("Back", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
                {
                    cmds.remove<OnlineMenu>(ent);
                    cmds.add(ent, state.mainMenu);
                }
            }

            ImGui::End();
        }
    });

    cubos.system("show offline menu").tagged(imguiTag).call([](Commands cmds, Query<Entity, OfflineMenu&> query) {
        for (auto [ent, state] : query)
        {
            // Center ImGui window
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2),
                                    ImGuiCond_Always, ImVec2(0.5F, 0.5F));
            if (!ImGui::Begin("Offline Play", nullptr,
                              ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
                                  ImGuiWindowFlags_AlwaysAutoResize))
            {
                ImGui::End();
                continue;
            }

            ImGui::Text("Make sure to have at least two teams with at least one player each");

            ImGui::Separator();

            std::unordered_set<int> usedPlayerIds{};
            for (auto& team : state.teams)
            {
                for (auto& player : team.players)
                {
                    usedPlayerIds.insert(player.id);
                }
            }

            for (int i = 0; i < state.teams.size(); ++i)
            {
                ImGui::PushID(i);
                ImGui::Text("Team %d:", i + 1);
                if (i > 1)
                {
                    ImGui::SameLine();
                    if (ImGui::Button("Remove"))
                    {
                        state.teams.erase(state.teams.begin() + i);
                        i -= 1;
                        ImGui::PopID();
                        continue;
                    }
                }

                ImGui::Indent();

                for (int j = 0; j < state.teams[i].players.size(); ++j)
                {
                    ImGui::PushID(j);
                    ImGui::Text("Player %d", state.teams[i].players[j].id);
                    ImGui::SameLine();
                    if (ImGui::Button("Remove"))
                    {
                        state.teams[i].players.erase(state.teams[i].players.begin() + j);
                        j -= 1;
                        ImGui::PopID();
                        continue;
                    }
                    ImGui::PopID();
                }

                if (ImGui::Button("Add Player"))
                {
                    int id = 1;
                    while (usedPlayerIds.count(id) > 0)
                    {
                        id += 1;
                    }
                    state.teams[i].players.push_back({id});
                    usedPlayerIds.insert(id);
                }

                ImGui::Unindent();
                ImGui::PopID();
            }

            if (ImGui::Button("Add Team"))
            {
                state.teams.push_back({});
            }

            ImGui::Separator();

            bool emptyTeams = false;
            for (auto& team : state.teams)
            {
                if (team.players.empty())
                {
                    emptyTeams = true;
                    break;
                }
            }

            if (ImGui::Button("Start", ImVec2(ImGui::GetContentRegionAvail().x, 0)) && state.teams.size() > 1 &&
                !emptyTeams)
            {
                cmds.remove<OfflineMenu>(ent);
                cmds.add(ent, RestartMenu{.mainMenu = state.mainMenu});
                cmds.add(ent, state.mainMenu.levelGenerator);

                for (int i = 0; i < state.teams.size(); ++i)
                {
                    TeamSpawner spawner{};
                    spawner.boat = state.mainMenu.boats[i % state.mainMenu.boats.size()];
                    for (int j = 0; j < state.teams[i].players.size(); ++j)
                    {
                        TeamSpawner::Player player{};
                        player.id = state.teams[i].players[j].id;
                        player.scene = state.mainMenu.characters[j % state.mainMenu.characters.size()];
                        spawner.players.push_back(player);
                    }
                    cmds.create().named("team").add(spawner).relatedTo(ent, ChildOf{});
                }
            }
            if (ImGui::Button("Back", ImVec2(ImGui::GetContentRegionAvail().x, 0)))
            {
                cmds.remove<OfflineMenu>(ent);
                cmds.add(ent, state.mainMenu);
            }

            ImGui::End();
        }
    });

    cubos.system("show restart menu")
        .tagged(imguiTag)
        .call([](Commands cmds, Query<Entity, RestartMenu&> query, Query<Entity, const TeamSpawner&> teams) {
            for (auto [ent, state] : query)
            {
                int count = 0;
                for (auto [teamEnt, team] : teams)
                {
                    count += 1;
                }

                if (count <= 1)
                {
                    for (auto [teamEnt, team] : teams)
                    {
                        cmds.add(teamEnt, DestroyTree{});
                    }

                    cmds.remove<LevelGenerator>(ent);
                    cmds.remove<RestartMenu>(ent);
                    cmds.add(ent, state.mainMenu);
                }
            }
        });
}
