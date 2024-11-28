#include "plugin.hpp"

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

static const int size = 5000;
static const int assetSize = 100;
static const int ENTITY_AMOUNT = 10;
static const int chunkNum = size / assetSize; // N = 10
static const float minRnd = -5.0F;
static const float maxRnd = 5.0F;

// tTODO: meter colider, camara

namespace airships::client
{
    void islandsPlugin(Cubos& cubos)
    {
        cubos.depends(assetsPlugin);

        cubos.startupSystem("islands").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            std::mt19937 engine{std::random_device()()};
            std::uniform_real_distribution distCoord(minRnd, maxRnd);
            std::uniform_int_distribution distAssetType(0, static_cast<int>(islands.size() - 1));

            std::map<int, glm::vec3> chunksTaken; // to store the chunk id to avoid overlapping models
            for (int i = 0; i < ENTITY_AMOUNT; i++)
            {
                auto blueprint = cmds.spawn(assets.read(islands[distAssetType(engine)])->blueprint);
                float cx = distCoord(engine);
                float cz = distCoord(engine);
                float x = assetSize * cx + (assetSize / 2);
                float y = distCoord(engine);
                float z = -(assetSize * cz + (assetSize / 2));
                float cid;
                glm::vec3 pos;
                do
                {
                    pos = {x / 5, y, z / 5};
                    cid = cz * chunkNum + cx;
                } while (chunksTaken.find(cid) != chunksTaken.end());
                chunksTaken[cid] = pos;
                blueprint.add("player", Position{.vec = pos});
            }
        });
    }
} // namespace airships::client
