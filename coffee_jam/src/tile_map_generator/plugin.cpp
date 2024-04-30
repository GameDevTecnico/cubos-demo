#include "plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../object/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::TileMapGenerator)
{
    return cubos::core::ecs::TypeBuilder<TileMapGenerator>("demo::TileMapGenerator")
        .withField("seed", &TileMapGenerator::seed)
        .withField("mapSide", &TileMapGenerator::mapSide)
        .withField("chunkSide", &TileMapGenerator::chunkSide)
        .withField("tileSide", &TileMapGenerator::tileSide)
        .withField("grass", &TileMapGenerator::grass)
        .withField("roadLine", &TileMapGenerator::roadLine)
        .withField("roadSimple", &TileMapGenerator::roadSimple)
        .withField("sidewalk", &TileMapGenerator::sidewalk)
        .withField("crate", &TileMapGenerator::crate)
        .withField("fenceStraight", &TileMapGenerator::fenceStraight)
        .withField("fenceCurve", &TileMapGenerator::fenceCurve)
        .withField("wallStraight", &TileMapGenerator::wallStraight)
        .withField("wallCurve", &TileMapGenerator::wallCurve)
        .build();
}

void demo::tileMapGeneratorPlugin(Cubos& cubos)
{
    cubos.depends(tileMapPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(transformPlugin);

    cubos.component<TileMapGenerator>();

    cubos.observer("generate TileMap")
        .onAdd<TileMapGenerator>()
        .call([](Commands cmds, Assets& assets, Query<Entity, const TileMapGenerator&> query) {
            for (auto [entity, generator] : query)
            {
                TileMap map{
                    .chunkSide = generator.chunkSide,
                    .tileSide = generator.tileSide,
                    .floorTypes =
                        {
                            generator.grass,
                            generator.roadLine,
                            generator.roadSimple,
                            generator.sidewalk,
                        },
                    .wallTypes =
                        {
                            generator.fenceStraight,
                            generator.fenceCurve,
                            generator.wallStraight,
                            generator.wallCurve,
                        },
                };

                std::mt19937 mt(generator.seed);
                std::uniform_real_distribution<float> randomDist(0.0F, 1.0F);

                map.floorTiles.resize(generator.mapSide, std::vector<Tile>(generator.mapSide, Tile{0, 0}));
                map.wallTiles.resize(generator.mapSide, std::vector<Tile>(generator.mapSide, Tile{UINT8_MAX, 0}));

                for (int ty = 0; ty < generator.mapSide; ++ty)
                {
                    for (int tx = 0; tx < generator.mapSide; ++tx)
                    {
                        int rx = tx % 24;
                        if (rx == 8)
                        {
                            map.floorTiles[ty][tx].type = 1;
                        }
                        else if (rx >= 7 && rx <= 9)
                        {
                            map.floorTiles[ty][tx].type = 2;
                        }
                        else if (rx >= 4 && rx <= 12)
                        {
                            map.floorTiles[ty][tx].type = 3;
                        }

                        if (rx == 0)
                        {
                            map.wallTiles[ty][tx] = {.type = 0, .rotation = 1};
                        }
                        else if (randomDist(mt) < 0.01F)
                        {
                            auto crate = cmds.spawn(assets.read(generator.crate)->blueprint).entity("crate");
                            cmds.relate(crate, entity, ChildOf{});
                            cmds.add(crate, Object{.position = {tx, ty}, .size = {1, 1}});
                        }
                    }
                }

                cmds.add(entity, std::move(map));
                cmds.remove<TileMapGenerator>(entity);
            }
        });
}
