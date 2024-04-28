#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::TileMap)
{
    return cubos::core::ecs::TypeBuilder<TileMap>("demo::TileMap")
        .withField("chunkSide", &TileMap::chunkSide)
        .withField("types", &TileMap::types)
        .withField("tiles", &TileMap::tiles)
        .withField("entities", &TileMap::entities)
        .build();
}

void demo::tileMapPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(voxelsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(rendererPlugin);

    cubos.component<TileMap>();

    cubos.observer("load and spawn TileMap chunks")
        .onAdd<TileMap>()
        .call([](Commands cmds, Query<Entity, TileMap&> query, Assets& assets) {
            for (auto [entity, map] : query)
            {
                // Load all tile voxel grids.
                std::vector<AssetRead<VoxelGrid>> tileGrids;
                for (auto asset : map.types)
                {
                    tileGrids.emplace_back(std::move(assets.read(asset)));
                    CUBOS_ASSERT((tileGrids.back()->size() == glm::uvec3{8, 1, 8}), "Tile voxel grids must be 8x1x8");
                }

                CUBOS_ASSERT(map.tiles.size() % map.chunkSide == 0, "Tile-map size must be a multiple of chunk side");
                auto mapSide = map.tiles.size() / map.chunkSide;
                map.entities.resize(mapSide, std::vector<Entity>(mapSide, Entity{}));

                // Create a grid of chunk voxel grids which we will fill below.
                std::vector<std::vector<VoxelGrid>> chunks(mapSide);
                for (size_t y = 0; y < mapSide; ++y)
                {
                    for (size_t x = 0; x < mapSide; ++x)
                    {
                        chunks[y].emplace_back(glm::uvec3{8 * map.chunkSide, 1, 8 * map.chunkSide});
                    }
                }

                // Initialize all chunk voxel grids.
                CUBOS_INFO("Generating chunks");
                for (size_t y = 0; y < map.tiles.size(); ++y)
                {
                    CUBOS_ASSERT(map.tiles[y].size() == map.tiles.size(), "Tile-map must be square");

                    for (size_t x = 0; x < map.tiles[y].size(); ++x)
                    {
                        auto type = map.tiles[y][x];
                        CUBOS_ASSERT(tileGrids.size() > type, "No such tile type {}", type);

                        for (size_t tx = 0; tx < 8; ++tx)
                        {
                            for (size_t tz = 0; tz < 8; ++tz)
                            {
                                chunks[y / map.chunkSide][x / map.chunkSide].set(
                                    glm::uvec3{tx + 8 * (x % map.chunkSide), 0, tz + 8 * (y % map.chunkSide)},
                                    tileGrids[type]->get(glm::uvec3{tx, 0, tz}));
                            }
                        }
                    }
                }

                // Spawn entities for each chunk.
                CUBOS_INFO("Spawning chunks");
                auto chunksRoot = cmds.create().named("chunks").entity();
                cmds.relate(chunksRoot, entity, ChildOf{});
                for (size_t y = 0; y < mapSide; ++y)
                {
                    for (size_t x = 0; x < mapSide; ++x)
                    {
                        auto asset = assets.create(std::move(chunks[y][x]));
                        auto chunk = cmds.create()
                                         .add(RenderableGrid{.asset = asset, .offset = glm::vec3{0, -1.0F, 0}})
                                         .add(Position{glm::vec3{8 * map.chunkSide * x, 0, 8 * map.chunkSide * y}})
                                         .entity();
                        cmds.relate(chunk, chunksRoot, ChildOf{});
                    }
                }

                CUBOS_INFO("Done generating TileMap");
            }
        });
}
