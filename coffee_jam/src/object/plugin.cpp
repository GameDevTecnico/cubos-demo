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
        .withField("force", &Object::force)
        .build();
}

void demo::objectPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(tileMapPlugin);

    cubos.component<Object>();

    cubos.system("initialize Object positions")
        .call([](Commands cmds, Query<Entity, Position&, Rotation&, Object&, const ChildOf&, TileMap&> query) {
            for (auto [ent, position, rotation, object, childOf, map] : query)
            {
                if (object.initialized)
                {
                    continue;
                }

                // Change position until free space is found.
                while (true)
                {
                    bool free = true;
                    for (int y = object.position.y; y < object.position.y + object.size.y; ++y)
                    {
                        for (int x = object.position.x; x < object.position.x + object.size.x; ++x)
                        {
                            if (!map.entities[y][x].isNull() && map.entities[y][x] != ent)
                            {
                                if (object.force)
                                {
                                    // Will try again next frame.
                                    cmds.destroy(map.entities[y][x]);
                                    return;
                                }

                                free = false;
                                break;
                            }
                        }

                        if (!free)
                        {
                            break;
                        }
                    }

                    if (free)
                    {
                        break;
                    }

                    int direction = rand() % 4;
                    object.position.x += direction == 0 ? 1 : direction == 1 ? -1 : 0;
                    object.position.y += direction == 2 ? 1 : direction == 3 ? -1 : 0;

                    if (object.position.x >= map.floorTiles.size())
                    {
                        object.position.x = 0;
                    }
                    if (object.position.y >= map.floorTiles.size())
                    {
                        object.position.y = 0;
                    }
                }

                // Mark the tile as occupied.
                for (int y = object.position.y; y < object.position.y + object.size.y; ++y)
                {
                    for (int x = object.position.x; x < object.position.x + object.size.x; ++x)
                    {
                        map.entities[y][x] = ent;
                    }
                }

                auto tileSide = static_cast<float>(map.tileSide);
                position.vec.x = tileSide / 2.0F + tileSide * static_cast<float>(object.position.x);
                position.vec.y = 0.0F;
                position.vec.z = tileSide / 2.0F + tileSide * static_cast<float>(object.position.y);
                rotation.quat = glm::identity<glm::quat>();

                object.initialized = true;
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
