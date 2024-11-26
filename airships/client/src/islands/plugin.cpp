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

using namespace cubos::engine;

static const Asset<Scene> IslandSceneAsset = AnyAsset("a88d914b-f1d7-463b-9a20-cf76bb5508d6");
static const int size = 1000;
static const int assetSize = 100;
static const int ENTITY_AMOUNT = 6;
static const int chunkNum = size / assetSize; // N = 10
static const float minRnd = -5.0F;
static const float maxRnd = 5.0F;

namespace airships::client
{
    void islandsPlugin(Cubos& cubos)
    {
        cubos.depends(assetsPlugin);

        cubos.startupSystem("islands").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            std::mt19937 engine{std::random_device()()};
            std::uniform_real_distribution dist(minRnd, maxRnd);

            std::map<int, glm::vec3> chunksTaken; // to store the chunk id to avoid overlapping models
            for (int i = 0; i < ENTITY_AMOUNT; i++)
            {
                auto blueprint = cmds.spawn(assets.read(IslandSceneAsset)->blueprint);
                float cx = dist(engine);
                float cz = dist(engine);
                float x = assetSize * cx + (assetSize / 2);
                float y = dist(engine);
                float z = assetSize * cz + (assetSize / 2);
                float cid;
                glm::vec3 pos;
                do
                {
                    pos = {x, y, -z};
                    cid = cz * chunkNum + cx;
                } while (chunksTaken.find(cid) != chunksTaken.end());
                chunksTaken[cid] = pos;
                blueprint.add("player", Position{.vec = pos});
            }
        });
    }
} // namespace airships::client
