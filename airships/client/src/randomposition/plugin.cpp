#include "plugin.hpp"

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/type.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <random>

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
        .build();
}

namespace airships::client
{
    void randompositionPlugin(Cubos& cubos)
    {
        cubos.resource<ChunkInfo>();
        cubos.component<RandomPosition>();

        cubos.observer("associate random position")
            .onAdd<RandomPosition>()
            .call([](Commands cmds, Query<Entity, const RandomPosition&> query, ChunkInfo& chunkInfo) {
                for (auto [ent, _] : query)
                {
                    int cid;
                    int x, y, z;
                    do 
                    {
                    std::mt19937 engine{std::random_device()()};
                    std::uniform_int_distribution distCoord(chunkInfo.minRnd, chunkInfo.maxRnd);
                    x = distCoord(engine);
                    y = distCoord(engine);
                    z = distCoord(engine);
                    cid = (100 * z) + (10 * y) + x;
                    } while (chunkInfo.chunksTaken.find(cid) != chunkInfo.chunksTaken.end());
                    
                    x *= chunkInfo.chunkSize;
                    y *= chunkInfo.chunkSize;
                    z *= chunkInfo.chunkSize;

                    glm::vec3 pos = {x, y, z};
                    chunkInfo.chunksTaken[cid] = pos;
                    cmds.add(ent, Position{.vec = pos});
                }
            });
    }
} // namespace airships::client

