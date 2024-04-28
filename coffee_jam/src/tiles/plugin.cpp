#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/unordered_map.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::TileMap)
{
    return cubos::core::ecs::TypeBuilder<TileMap>("demo::TileMap")
        .withField("types", &TileMap::types)
        .withField("tiles", &TileMap::tiles)
        .build();
}

void demo::tilesPlugin(Cubos& cubos)
{
    cubos.component<TileMap>();

    cubos.depends(assetsPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(transformPlugin);

    cubos.observer("unpack TileMap")
        .onAdd<TileMap>()
        .call([](Commands cmds, Query<Entity, const TileMap&> query, Assets& assets) {
            for (auto [entity, tileMap] : query)
            {
                // Load all scenes.
                std::unordered_map<std::string, Scene> tileScenes;
                for (auto [type, asset] : tileMap.types)
                {
                    tileScenes[type] = *assets.read(asset);
                }

                for (size_t y = 0; y < tileMap.tiles.size(); ++y)
                {
                    for (size_t x = 0; x < tileMap.tiles[y].size(); ++x)
                    {
                        auto type = tileMap.tiles[y][x];
                        CUBOS_ASSERT(tileScenes.contains(type), "No such tile type {}", type);
                        auto root = cmds.spawn(tileScenes[type].blueprint)
                                        .add("root", Position{glm::vec3{x * 16.0F, 0.0F, y * 16.0F}})
                                        .entity("root");
                        cmds.relate(root, entity, ChildOf{});
                    }
                }
            }
        });
}
