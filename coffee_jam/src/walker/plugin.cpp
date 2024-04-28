#include "plugin.hpp"
#include "../tile_map/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Walker)
{
    return cubos::core::ecs::TypeBuilder<Walker>("demo::Walker")
        .withField("position", &Walker::position)
        .withField("direction", &Walker::direction)
        .withField("jumpHeight", &Walker::jumpHeight)
        .withField("moveSpeed", &Walker::moveSpeed)
        .withField("halfRotationTime", &Walker::halfRotationTime)
        .withField("progress", &Walker::progress)
        .build();
}

void demo::walkerPlugin(Cubos& cubos)
{
    cubos.depends(tileMapPlugin);
    cubos.depends(transformPlugin);

    cubos.component<Walker>();

    cubos.system("do Walker movement")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Query<Entity, Position&, Rotation&, Walker&, const ChildOf&, TileMap&> query) {
            for (auto [ent, position, rotation, walker, childOf, map] : query)
            {
                auto tileSide = static_cast<float>(map.tileSide);

                if (!walker.initialized)
                {
                    // If the initial position is already occupied, try to find an empty one nearby.
                    walker.position.x = glm::clamp(walker.position.x, 0, static_cast<int>(map.tiles.size() - 1));
                    walker.position.y = glm::clamp(walker.position.y, 0, static_cast<int>(map.tiles.size() - 1));
                    while (!map.entities[walker.position.y][walker.position.x].isNull())
                    {
                        walker.position += glm::ivec2{1, 0};
                        if (walker.position.x >= map.tiles.size())
                        {
                            walker.position.x = 0;
                            walker.position.y += 1;
                        }
                        if (walker.position.y >= map.tiles.size())
                        {
                            walker.position.y = 0;
                        }
                    }
                    map.entities[walker.position.y][walker.position.x] = ent;

                    // Set the actual position of the entity.
                    position.vec.x = tileSide / 2.0F + tileSide * static_cast<float>(walker.position.x);
                    position.vec.y = 0.0F;
                    position.vec.z = tileSide / 2.0F + tileSide * static_cast<float>(walker.position.y);
                    walker.initialized = true;
                }

                if (walker.direction == glm::ivec2{0, 0})
                {
                    continue; // Entity is not moving, skip it.
                }

                // Check if movement is valid.
                auto targetTile = walker.position + walker.direction;
                if (targetTile.x < 0 || targetTile.y < 0 || targetTile.x >= map.tiles.size() ||
                    targetTile.y >= map.tiles.size() ||
                    (!map.entities[targetTile.y][targetTile.x].isNull() &&
                     map.entities[targetTile.y][targetTile.x] != ent))
                {
                    // There's already an entity in the target tile, stop the movement.
                    walker.direction = {0, 0};
                    continue;
                }

                // Occupy the target tile.
                map.entities[targetTile.y][targetTile.x] = ent;

                // Get the source and target positions of the entity's movement.
                glm::vec2 source = static_cast<glm::vec2>(walker.position);
                glm::vec2 target = static_cast<glm::vec2>(walker.position + walker.direction);

                // Increase the progress value and calculate the new position of the entity.
                walker.progress = glm::clamp(walker.progress + dt.value() * walker.moveSpeed, 0.0F, 1.0F);
                position.vec.x = tileSide / 2.0F + tileSide * glm::mix(source.x, target.x, walker.progress);
                position.vec.y = glm::mix(0.0F, walker.jumpHeight, glm::sin(walker.progress * glm::pi<float>()));
                position.vec.z = tileSide / 2.0F + tileSide * glm::mix(source.y, target.y, walker.progress);

                // Set the entity's rotation as appropriate.
                auto targetRotation =
                    glm::quatLookAt(-glm::normalize(glm::vec3(static_cast<float>(walker.direction.x), 0.0F,
                                                              static_cast<float>(walker.direction.y))),
                                    glm::vec3{0.0F, 1.0F, 0.0F});
                float rotationAlpha = 1.0F - glm::pow(0.5F, dt.value() / walker.halfRotationTime);
                rotation.quat = glm::slerp(rotation.quat, targetRotation, rotationAlpha);

                // If the entity has reached the target position, reset the direction.
                if (walker.progress == 1.0F)
                {
                    // Clear the source tile.
                    map.entities[walker.position.y][walker.position.x] = Entity{};

                    walker.position += walker.direction;
                    walker.direction = {0, 0};
                    walker.progress = 0.0F;
                }
            }
        });
}
