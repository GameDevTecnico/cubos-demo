#include "plugin.hpp"
#include "../tile_map/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Movement)
{
    return cubos::core::ecs::TypeBuilder<Movement>("demo::Movement")
        .withField("position", &Movement::position)
        .withField("direction", &Movement::direction)
        .withField("facing", &Movement::facing)
        .withField("moveSpeed", &Movement::moveSpeed)
        .withField("halfRotationTime", &Movement::halfRotationTime)
        .withField("minPosition", &Movement::minPosition)
        .withField("maxPosition", &Movement::maxPosition)
        .withField("progress", &Movement::progress)
        .build();
}

void demo::movementPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(tileMapPlugin);

    cubos.component<Movement>();


    cubos.system("player movement handler")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Query<Entity, Position&, Rotation&, Movement&, const ChildOf&, TileMap&> query) {
            for (auto [ent, position, rotation, movement, childOf, map] : query)
            {
                auto tileSide = static_cast<float>(map.tileSide);

                if (!movement.initialized)
                {
                    // If the initial position is already occupied, try to find an empty one nearby.
                    movement.position.x = glm::clamp(movement.position.x, 0, static_cast<int>(map.tiles.size() - 1));
                    movement.position.y = glm::clamp(movement.position.y, 0, static_cast<int>(map.tiles.size() - 1));
                    while (!map.entities[movement.position.y][movement.position.x].isNull())
                    {
                        movement.position += glm::ivec2{1, 0};
                        int r = rand() % 2;
                        movement.position += (rand() % 2 == 0 ? -1 : 1) * glm::ivec2{r, 1 - r};
                        if (movement.position.x >= map.tiles.size())
                        {
                            movement.position.x -= map.tiles.size();
                        }
                        if (movement.position.y >= map.tiles.size())
                        {
                            movement.position.y -= map.tiles.size();
                        }
                    }
                    map.entities[movement.position.y][movement.position.x] = ent;

                    // Set the actual position of the entity.
                    position.vec.x = tileSide / 2.0F + tileSide * static_cast<float>(movement.position.x);
                    position.vec.y = 0.0F;
                    position.vec.z = tileSide / 2.0F + tileSide * static_cast<float>(movement.position.y);
                    movement.initialized = true;
                }

                auto targetTile = movement.position + movement.direction;
                if (movement.direction != glm::ivec2{0, 0})
                {
                    movement.facing = movement.direction;

                    // Check if movement is valid.
                    if (targetTile.x < 0 || targetTile.y < 0 || targetTile.x >= map.tiles.size() ||
                        targetTile.y >= map.tiles.size() || targetTile.x < movement.minPosition.x ||
                        targetTile.y < movement.minPosition.y || targetTile.x > movement.maxPosition.x ||
                        targetTile.y > movement.maxPosition.y ||
                        (!map.entities[targetTile.y][targetTile.x].isNull() &&
                         map.entities[targetTile.y][targetTile.x] != ent))
                    {
                        // There's already an entity in the target tile, or its out of bounsd, stop the movement.
                        movement.direction = {0, 0};
                    }
                }

                if (movement.direction != glm::ivec2{0, 0})
                {
                    // Occupy the target tile.
                    map.entities[targetTile.y][targetTile.x] = ent;

                    // Get the source and target positions of the entity's movement.
                    glm::vec2 source = static_cast<glm::vec2>(movement.position);
                    glm::vec2 target = static_cast<glm::vec2>(movement.position + movement.direction);

                    // Increase the progress value and calculate the new position of the entity.
                    movement.progress = glm::clamp(movement.progress + dt.value() * movement.moveSpeed, 0.0F, 1.0F);
                    position.vec.x = tileSide / 2.0F + tileSide * glm::mix(source.x, target.x, movement.progress);
                    /*position.vec.y = glm::mix(0.0F, movement.jumpHeight, glm::sin(movement.progress * glm::pi<float>()));*/
                    position.vec.z = tileSide / 2.0F + tileSide * glm::mix(source.y, target.y, movement.progress);

                    // If the entity has reached the target position, reset the direction.
                    if (movement.progress == 1.0F)
                    {
                        // Clear the source tile.
                        map.entities[movement.position.y][movement.position.x] = Entity{};

                        movement.position += movement.direction;
                        movement.direction = {0, 0};
                        movement.progress = 0.0F;
                    }
                }

                // Set the entity's rotation as appropriate.
                auto targetRotation =
                    glm::quatLookAt(-glm::normalize(glm::vec3(static_cast<float>(movement.facing.x), 0.0F,
                                                              static_cast<float>(movement.facing.y))),
                                    glm::vec3{0.0F, 1.0F, 0.0F});
                float rotationAlpha = 1.0F - glm::pow(0.5F, dt.value() / movement.halfRotationTime);
                rotation.quat = glm::slerp(rotation.quat, targetRotation, rotationAlpha);
            }
        });

    cubos.observer("clear up Movement positions")
        .onRemove<Movement>()
        .call([](Query<Entity, Movement&, const ChildOf&, TileMap&> query) {
            for (auto [ent, movement, childOf, map] : query)
            {
                // Mark the tile as free.
                CUBOS_ASSERT(map.entities[movement.position.y][movement.position.x] == ent,
                             "Tile is not occupied by the entity");
                map.entities[movement.position.y][movement.position.x] = {};

                auto target = movement.position + movement.direction;
                if (target.x >= 0 && target.y >= 0 && target.x < map.tiles.size() &&
                    target.y < map.tiles.size() && map.entities[target.y][target.x] == ent)
                {
                    map.entities[target.y][target.x] = {};
                }
            }
        });
}
