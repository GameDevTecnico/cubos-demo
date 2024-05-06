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

CUBOS_REFLECT_IMPL(demo::Tile)
{
    return cubos::core::ecs::TypeBuilder<Tile>("demo::Tile")
        .withField("type", &Tile::type)
        .withField("rotation", &Tile::rotation)
        .build();
}

CUBOS_REFLECT_IMPL(demo::TileMap)
{
    return cubos::core::ecs::TypeBuilder<TileMap>("demo::TileMap")
        .withField("chunkSide", &TileMap::chunkSide)
        .withField("tileSide", &TileMap::tileSide)
        .withField("wallHeight", &TileMap::wallHeight)
        .withField("floorTypes", &TileMap::floorTypes)
        .withField("wallTypes", &TileMap::wallTypes)
        .withField("floorTiles", &TileMap::floorTiles)
        .withField("wallTiles", &TileMap::wallTiles)
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
                std::vector<AssetRead<VoxelGrid>> floorTileGrids;
                std::vector<AssetRead<VoxelGrid>> wallTileGrids;
                for (auto asset : map.floorTypes)
                {
                    floorTileGrids.emplace_back(std::move(assets.read(asset)));
                    CUBOS_ASSERT(
                        (floorTileGrids.back()->size() == glm::uvec3{map.tileSide, 1, map.tileSide}),
                        "Floor tile voxel grids size must match the specified tile size and have a height of 1 ({})",
                        map.tileSide);
                }
                for (auto asset : map.wallTypes)
                {
                    wallTileGrids.emplace_back(std::move(assets.read(asset)));
                    CUBOS_ASSERT(
                        (wallTileGrids.back()->size() == glm::uvec3{map.tileSide, map.wallHeight, map.tileSide}),
                        "Floor tile voxel grids size must match the specified tile size and have a height of 1 ({})",
                        map.tileSide);
                }

                CUBOS_ASSERT(map.floorTiles.size() % map.chunkSide == 0,
                             "Tile-map size must be a multiple of chunk side");
                CUBOS_ASSERT(map.floorTiles.size() == map.wallTiles.size(),
                             "Floor and wall tile-maps must be the same size");
                auto mapSide = map.floorTiles.size() / map.chunkSide;
                map.entities.resize(map.floorTiles.size(), std::vector<Entity>(map.floorTiles.size(), Entity{}));

                // Create a grid of chunk voxel grids which we will fill below.
                std::vector<std::vector<VoxelGrid>> chunks(mapSide);
                for (size_t y = 0; y < mapSide; ++y)
                {
                    for (size_t x = 0; x < mapSide; ++x)
                    {
                        chunks[y].emplace_back(
                            glm::uvec3{map.tileSide * map.chunkSide, map.wallHeight + 1, map.tileSide * map.chunkSide});
                    }
                }

                // Initialize all chunk voxel grids.
                CUBOS_INFO("Generating chunks");
                for (size_t y = 0; y < map.floorTiles.size(); ++y)
                {
                    CUBOS_ASSERT(map.floorTiles[y].size() == map.floorTiles.size(), "Tile-map must be square");
                    CUBOS_ASSERT(map.wallTiles[y].size() == map.floorTiles.size(), "Tile-map must be square");

                    for (size_t x = 0; x < map.floorTiles[y].size(); ++x)
                    {
                        auto floor = map.floorTiles[y][x];
                        auto wall = map.wallTiles[y][x];
                        CUBOS_ASSERT(floorTileGrids.size() > floor.type, "No such floor type {}", floor.type);
                        CUBOS_ASSERT(wallTileGrids.size() > wall.type || wall.type == UINT8_MAX, "No such wall type {}",
                                     wall.type);
                        floor.rotation = floor.rotation % 4;
                        wall.rotation = wall.rotation % 4;

                        for (size_t tx = 0; tx < map.tileSide; ++tx)
                        {
                            for (size_t tz = 0; tz < map.tileSide; ++tz)
                            {
                                // Map the tile coordinates to the actual model coordinates, considering the tile's
                                // rotation.
                                size_t mx = tx;
                                size_t mz = tz;
                                if (floor.rotation == 1)
                                {
                                    std::swap(mx, mz);
                                    mz = map.tileSide - mz - 1;
                                }
                                else if (floor.rotation == 2)
                                {
                                    mx = map.tileSide - mx - 1;
                                    mz = map.tileSide - mz - 1;
                                }
                                else if (floor.rotation == 3)
                                {
                                    std::swap(mx, mz);
                                    mx = map.tileSide - mx - 1;
                                }

                                chunks[y / map.chunkSide][x / map.chunkSide].set(
                                    glm::uvec3{tx + map.tileSide * (x % map.chunkSide), 0,
                                               tz + map.tileSide * (y % map.chunkSide)},
                                    floorTileGrids[floor.type]->get(glm::uvec3{mx, 0, mz}));

                                mx = tx;
                                mz = tz;
                                if (wall.rotation == 1)
                                {
                                    std::swap(mx, mz);
                                    mz = map.tileSide - mz - 1;
                                }
                                else if (wall.rotation == 2)
                                {
                                    mx = map.tileSide - mx - 1;
                                    mz = map.tileSide - mz - 1;
                                }
                                else if (wall.rotation == 3)
                                {
                                    std::swap(mx, mz);
                                    mx = map.tileSide - mx - 1;
                                }

                                if (wall.type != UINT8_MAX)
                                {
                                    for (size_t ty = 1; ty <= map.wallHeight; ++ty)
                                    {
                                        chunks[y / map.chunkSide][x / map.chunkSide].set(
                                            glm::uvec3{tx + map.tileSide * (x % map.chunkSide), ty,
                                                       tz + map.tileSide * (y % map.chunkSide)},
                                            wallTileGrids[wall.type]->get(glm::uvec3{mx, ty - 1, mz}));
                                    }

                                    map.entities[y][x] = entity; // Mark the tile as occupied (by the map itself).
                                }
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
                                         .add(Position{glm::vec3{map.tileSide * map.chunkSide * x, 0,
                                                                 map.tileSide * map.chunkSide * y}})
                                         .entity();
                        cmds.relate(chunk, chunksRoot, ChildOf{});
                    }
                }

                CUBOS_INFO("Done generating TileMap");
            }
        });
}
