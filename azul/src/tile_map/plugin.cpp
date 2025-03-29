#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

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
        .withField("types", &TileMap::types)
        .withField("tiles", &TileMap::tiles)
        .withField("entities", &TileMap::entities)
        .build();
}

void demo::tileMapPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(transformPlugin);

    cubos.component<TileMap>();

    cubos.observer("load and spawn TileMap tiles")
        .onAdd<TileMap>()
        .call([](Commands cmds, Query<Entity, TileMap&> query, Assets& assets) {
            for (auto [entity, map] : query)
            {
                // Load all tile scenes.
                std::vector<AssetRead<Scene>> tileScenes;
                for (auto asset : map.types)
                {
                    tileScenes.emplace_back(std::move(assets.read(asset)));
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
                        auto tileEnt =
                            cmds.spawn(*tileScenes[tile.type])
                                .add(Position{glm::vec3{x, tile.height * 0.5F, y}})
                                .add(Rotation{glm::angleAxis(glm::radians(90.0F * tile.rotation), glm::vec3{0, 1, 0})})
                                .entity();
                        cmds.relate(tileEnt, tilesRoot, ChildOf{});
                    }
                }

                CUBOS_INFO("Done generating TileMap");
            }
        });
}
