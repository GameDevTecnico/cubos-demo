#include "plugin.hpp"

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
        .withField("speed", &Walker::speed)
        .withField("progress", &Walker::progress)
        .build();
}

void demo::walkerPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.component<Walker>();

    cubos.observer("initialize Walker positions").onAdd<Walker>().call([](Commands cmds, Query<Entity, Walker&> query) {
        for (auto [ent, walker] : query)
        {
            cmds.add(ent, Position{glm::vec3{4.0F + 8.0F * static_cast<float>(walker.position.x), 0.0F,
                                             4.0F + 8.0F * static_cast<float>(walker.position.y)}});
        }
    });

    cubos.system("do Walker movement")
        .before(transformUpdateTag)
        .call([](const DeltaTime& dt, Query<Position&, Rotation&, Walker&> query) {
            for (auto [position, rotation, walker] : query)
            {
                if (walker.direction == glm::ivec2{0, 0})
                {
                    continue; // Entity is not moving, skip it.
                }

                // Get the source and target positions of the entity's movement.
                glm::vec2 source = static_cast<glm::vec2>(walker.position);
                glm::vec2 target = static_cast<glm::vec2>(walker.position + walker.direction);

                // Increase the progress value and calculate the new position of the entity.
                walker.progress = glm::clamp(walker.progress + dt.value() * walker.speed, 0.0F, 1.0F);
                position.vec.x = 4.0F + 8.0F * glm::mix(source.x, target.x, walker.progress);
                position.vec.y = glm::mix(0.0F, walker.jumpHeight, glm::sin(walker.progress * glm::pi<float>()));
                position.vec.z = 4.0F + 8.0F * glm::mix(source.y, target.y, walker.progress);

                // Set the entity's rotation as appropriate.
                rotation.quat = glm::quatLookAt(-glm::normalize(glm::vec3(static_cast<float>(walker.direction.x), 0.0F,
                                                                          static_cast<float>(walker.direction.y))),
                                                glm::vec3{0.0F, 1.0F, 0.0F});

                // If the entity has reached the target position, reset the direction.
                if (walker.progress == 1.0F)
                {
                    walker.position += walker.direction;
                    walker.direction = {0, 0};
                    walker.progress = 0.0F;
                }
            }
        });
}
