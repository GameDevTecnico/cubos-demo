#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/assets/assets.hpp>

#include <vector>
#include <cmath>

#include <glm/glm.hpp>

#include "player.hpp"
#include "plugin.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void move(Query<Write<Player>, Write<PhysicsVelocity>, Write<Rotation>> query, Read<Input> input,
                 Read<DeltaTime> deltaTime, Write<Settings> settings)
{
    for (auto [entity, player, velocity, rotation] : query)
    {
        const float force = settings->getDouble("force", 5000.0F);
        const float jumpForce = settings->getDouble("jumpForce", 4000.0F);
        const float dragForce = settings->getDouble("dragForce", -2000.0F);
        const float rotationSpeed = settings->getDouble("rotationSpeed", 0.02F);
        const float maxSpeed = settings->getDouble("maxSpeed", 110.0F);

        auto moveForce = input->axis("move", player->id) * force;
        auto turn = -input->axis("turn", player->id) * rotationSpeed;

        float speed = glm::length(velocity->velocity);
        if (speed > maxSpeed)
        {
            auto direction = glm::normalize(velocity->velocity);
            velocity->velocity = direction * maxSpeed;
        }

        auto jump = input->pressed("jump", player->id);

        auto rotationDelta = glm::angleAxis(turn, glm::vec3{0.0F, 1.0F, 0.0F});
        rotation->quat = rotationDelta * rotation->quat;

        glm::vec3 forward = rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f);
        glm::vec3 side = rotation->quat * glm::vec3(1.0f, 0.0f, 0.0f);

        if (glm::length(side) > 3.0F)
        {
            velocity->force += side * dragForce;
        }
        if (moveForce == 0.0F && glm::length(forward) > 3.0F)
        {
            velocity->force += forward * dragForce;
        }
        velocity->force += forward * moveForce;
        // velocity->force += glm::vec3{0.0F, 1.0F, 0.0F} * 600.0F;

        if (jump && player->isOnGround)
        {
            velocity->impulse += glm::vec3{0.0F, 1.0F, 0.0F} * jumpForce;
            player->isOnGround = false;
        }
    }
}

// handle ground collision
// handle dead

void demo::playersPlugin(Cubos& cubos)
{
    cubos.addComponent<Player>();
    cubos.system(move).tagged("player.move").tagged("cubos.physics.apply_forces").before("cubos.transform.update");
}
