#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>

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
        .withField("tileSide", &TileMap::tileSide)
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
    cubos.depends(renderVoxelsPlugin);

    cubos.component<TileMap>();

    cubos.observer("load and spawn TileMap chunks")
        .onAdd<TileMap>()
        .call([](Commands cmds, Query<Entity, TileMap&> query, Assets& assets) {
            for (auto [entity, map] : query)
            {
                // Load all tile voxel grids.
                std::vector<AssetRead<VoxelGrid>> tileGrids;
                std::vector<AssetRead<VoxelGrid>> wallTileGrids;
                for (auto asset : map.types)
                {
                    tileGrids.emplace_back(std::move(assets.read(asset)));
                    CUBOS_ASSERT((tileGrids.back()->size() == glm::uvec3{map.tileSide, map.tileHeight, map.tileSide}),
                                 "Tile voxel grids size must match the specified tile size {} and have a height of {}",
                                 map.tileSide, map.tileHeight);
                }

                map.entities.resize(map.tiles.size(), std::vector<Entity>(map.tiles.size(), Entity{}));

                // Spawn entities for each tile.
                CUBOS_INFO("Spawning tiles");
                auto tilesRoot = cmds.create().named("tiles").entity();
                cmds.relate(tilesRoot, entity, ChildOf{});
                for (size_t y = 0; y < map.tiles.size(); ++y)
                {
                    for (size_t x = 0; x < map.tiles.size(); ++x)
                    {
                        const auto& tile = map.tiles[y][x];
                        auto chunk =
                            cmds.create()
                                .add(RenderVoxelGrid{.asset = map.types[tile.type]})
                                .add(Position{glm::vec3{map.tileSide * x, tile.height, map.tileSide * y}})
                                .add(Rotation{glm::angleAxis(glm::radians(90.0F * tile.rotation), glm::vec3{0, 1, 0})})
                                .entity();
                        cmds.relate(chunk, tilesRoot, ChildOf{});
                    }
                }

                CUBOS_INFO("Done generating TileMap");
            }
        });
}
