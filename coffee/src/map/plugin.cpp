#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::MapTile)
{
    return cubos::core::ecs::TypeBuilder<MapTile>("coffee::MapTile")
        .withField("scene", &MapTile::scene)
        .withField("rotation", &MapTile::rotation)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::Map)
{
    return cubos::core::ecs::TypeBuilder<Map>("coffee::Map")
        .withField("tiles", &Map::tiles)
        .withField("tileSize", &Map::tileSize)
        .build();
}

void coffee::mapPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(transformPlugin);

    cubos.component<Map>();

    cubos.observer("construct Map").onAdd<Map>().call([](Commands cmds, Assets& assets, Query<Entity, Map&> query) {
        for (auto [mapEnt, map] : query)
        {
            for (auto& [pos, tile] : map.tiles)
            {
                auto tilePos = glm::vec3{static_cast<float>(pos.x) * map.tileSize.x, 0.0F,
                                         static_cast<float>(pos.y) * map.tileSize.y};
                auto tileEnt =
                    cmds.spawn(assets.read(tile.scene)->blueprint())
                        .named("tile-" + std::to_string(pos.x) + "-" + std::to_string(pos.y))
                        .add(Position{tilePos})
                        .add(Rotation{glm::angleAxis(-glm::pi<float>() * 0.5F * static_cast<float>(tile.rotation),
                                                     glm::vec3{0.0F, 1.0F, 0.0F})})
                        .entity();
                cmds.relate(mapEnt, tileEnt, ChildOf{});
            }

            cmds.remove<Map>(mapEnt);
        }
    });
}
