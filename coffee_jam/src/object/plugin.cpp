#include "plugin.hpp"
#include "../tile_map/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Object)
{
    return cubos::core::ecs::TypeBuilder<Object>("demo::Object")
        .withField("position", &Object::position)
        .withField("size", &Object::size)
        .build();
}

void demo::objectPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(tileMapPlugin);

    cubos.component<Object>();

    cubos.system("initialize Object positions")
        .call([](Query<Entity, Position&, Rotation&, Object&, const ChildOf&, TileMap&> query) {
            for (auto [ent, position, rotation, object, childOf, map] : query)
            {
                if (object.initialized)
                {
                    continue;
                }

                object.initialized = true;

                auto tileSide = static_cast<float>(map.tileSide);
                position.vec.x = tileSide / 2.0F + tileSide * static_cast<float>(object.position.x);
                position.vec.y = 0.0F;
                position.vec.z = tileSide / 2.0F + tileSide * static_cast<float>(object.position.y);
                rotation.quat = glm::identity<glm::quat>();

                // Mark the tile as occupied.
                for (int y = object.position.y; y < object.position.y + object.size.y; ++y)
                {
                    for (int x = object.position.x; x < object.position.x + object.size.x; ++x)
                    {
                        CUBOS_ASSERT(map.entities[y][x].isNull() || map.entities[y][x] == ent,
                                     "Tile is already occupied by another entity");
                        map.entities[y][x] = ent;
                    }
                }
            }
        });

    cubos.observer("clear up Object positions")
        .onRemove<Object>()
        .call([](Query<Entity, Position&, Rotation&, Object&, const ChildOf&, TileMap&> query) {
            for (auto [ent, position, rotation, object, childOf, map] : query)
            {
                // Mark the tile as free.
                for (int y = object.position.y; y < object.position.y + object.size.y; ++y)
                {
                    for (int x = object.position.x; x < object.position.x + object.size.x; ++x)
                    {
                        CUBOS_ASSERT(map.entities[y][x] == ent, "Tile is not occupied by the entity");
                        map.entities[y][x] = {};
                    }
                }
            }
        });
}
