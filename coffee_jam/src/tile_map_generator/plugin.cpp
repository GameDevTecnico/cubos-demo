#include "plugin.hpp"
#include "../tile_map/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::TileMapGenerator)
{
    return cubos::core::ecs::TypeBuilder<TileMapGenerator>("demo::TileMapGenerator")
        .withField("seed", &TileMapGenerator::seed)
        .withField("mapSide", &TileMapGenerator::mapSide)
        .withField("chunkSide", &TileMapGenerator::chunkSide)
        .withField("grass", &TileMapGenerator::grass)
        .withField("roadLine", &TileMapGenerator::roadLine)
        .withField("roadSimple", &TileMapGenerator::roadSimple)
        .withField("sidewalk", &TileMapGenerator::sidewalk)
        .build();
}

void demo::tileMapGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(tileMapPlugin);

    cubos.component<TileMapGenerator>();

    cubos.observer("generate TileMap")
        .onAdd<TileMapGenerator>()
        .call([](Commands cmds, Query<Entity, const TileMapGenerator&> query) {
            for (auto [entity, generator] : query)
            {
                TileMap map{.chunkSide = generator.chunkSide,
                            .types = {
                                generator.grass,
                                generator.roadLine,
                                generator.roadSimple,
                                generator.sidewalk,
                            }};

                map.tiles.resize(generator.mapSide, std::vector<unsigned char>(generator.mapSide, 0));

                for (int ty = 0; ty < generator.mapSide; ++ty)
                {
                    for (int tx = 0; tx < generator.mapSide; ++tx)
                    {
                        int rx = tx % 24;
                        if (rx == 8)
                        {
                            map.tiles[ty][tx] = 1;
                        }
                        else if (rx >= 7 && rx <= 9)
                        {
                            map.tiles[ty][tx] = 2;
                        }
                        else if (rx >= 4 && rx <= 12)
                        {
                            map.tiles[ty][tx] = 3;
                        }
                    }
                }

                cmds.add(entity, std::move(map));
                cmds.remove<TileMapGenerator>(entity);
            }
        });
}
