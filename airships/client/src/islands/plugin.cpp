#include "plugin.hpp"
#include "../random_position/plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>

#include <random>
#include <iostream>
#include <unordered_set>
#include <memory>
#include <cstdint>
#include <map>
#include <array>

using namespace cubos::engine;

static const Asset<Scene> Island1SceneAsset = AnyAsset("c8fd19c1-438d-45d6-8395-300dc1809f7f");
static const Asset<Scene> Island2SceneAsset = AnyAsset("a796a308-0cad-4674-869d-c5e7177d711c");
static const Asset<Scene> Island3SceneAsset = AnyAsset("f136a3a7-6ac1-4fa7-a425-857f38746814");

static const std::array<Asset<Scene>, 3> islands = {Island1SceneAsset, Island2SceneAsset, Island3SceneAsset};

static const int ENTITY_AMOUNT = 30;

// tTODO: meter colider, camara

namespace airships::client
{
    void islandsPlugin(Cubos& cubos)
    {
        cubos.depends(randomPositionPlugin);
        cubos.depends(assetsPlugin);

        cubos.startupSystem("islands").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            std::mt19937 engine{std::random_device()()};
            std::uniform_int_distribution distAssetType(0, static_cast<int>(islands.size() - 1));

            for (int i = 0; i < ENTITY_AMOUNT; i++)
            {
                auto blueprint = cmds.spawn(assets.read(islands[distAssetType(engine)])->blueprint);
                blueprint.add("player", RandomPosition{});
            }
        });
    }
} // namespace airships::client
