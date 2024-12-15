#include "plugin.hpp"
#include "../storm/plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <random>
#include <cmath>

using namespace cubos::engine;
using namespace airships::client;

CUBOS_REFLECT_IMPL(ChunkInfo)
{
    return cubos::core::ecs::TypeBuilder<ChunkInfo>("airships::client::ChunkInfo")
        .withField("chunkSize", &ChunkInfo::chunkSize)
        .withField("minRnd", &ChunkInfo::minRnd)
        .withField("maxRnd", &ChunkInfo::maxRnd)
        .withField("chunksTaken", &ChunkInfo::chunksTaken)
        .build();
}

CUBOS_REFLECT_IMPL(RandomPosition)
{
    return cubos::core::ecs::TypeBuilder<RandomPosition>("airships::client::RandomPosition")
        .withField("startingPos", &RandomPosition::startingPos)
        .build();
}

namespace airships::client
{
    void randomPositionPlugin(Cubos& cubos)
    {
        cubos.depends(stormPlugin);
        cubos.resource<ChunkInfo>();
        cubos.component<RandomPosition>();

        cubos.observer("associate random position")
            .onAdd<RandomPosition>()
            .call([](Commands cmds, Query<Entity, RandomPosition&> query, ChunkInfo& chunkInfo, const StormInfo& si) {
                for (auto [ent, randomPosition] : query)
                {
                    std::mt19937 engine{std::random_device()()};
                    std::uniform_real_distribution randomAngle(0.0F, 2.0F * glm::pi<float>());
                    std::uniform_int_distribution distCoordY(-100, 100);

                    int cid;
                    int x, y, z;
                    do
                    {

                        x = std::round(cos(randomAngle(engine)) * si.stormRadius);
                        y = randomPosition.setYToZero ? 0 : distCoordY(engine);
                        z = std::round(sin(randomAngle(engine)) * si.stormRadius);
                        cid = (100 * (z / 100)) + (10 * (y / 100)) + (x / 100);
                    } while (chunkInfo.chunksTaken.find(cid) != chunkInfo.chunksTaken.end());

                    glm::vec3 pos = {x, y, z};
                    chunkInfo.chunksTaken[cid] = pos;
                    randomPosition.startingPos = pos;
                    cmds.add(ent, Position{.vec = pos});
                    cmds.add(ent, Rotation{.quat = glm::angleAxis(randomAngle(engine), glm::vec3(0.0F, 1.0F, 0.0F))});
                }
            });
    }
} // namespace airships::client
