#include <cubos/engine/settings/settings.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/collisions/collision_event.hpp>

#include <vector>
#include <cmath>

#include <glm/glm.hpp>

#include "../offset/offset.hpp"
#include "player.hpp"
#include "plugin.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EventReader;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void move(Query<Write<Player>, Write<Position>, Write<PhysicsVelocity>, Write<Rotation>> query,
                 Query<Write<Offset>> offsets, Read<Input> input, Read<DeltaTime> deltaTime, Write<Settings> settings,
                 EventReader<CollisionEvent> collisions)
{
    for (auto collision : collisions)
    {
        // Only handle collisions between players and other entities which are not players.
        if (!query[collision.entity] /* || query[collision.other]*/)
        {
            continue;
        }

        auto [player, position, velocity, rotation] = *query[collision.entity];
        position->vec -= collision.normal * collision.penetration;
        velocity->velocity -= collision.normal * glm::dot(collision.normal, velocity->velocity);

        if (collision.normal.y < -0.1F)
        {
            player->isOnGround = true;
        }
    }

    for (auto [entity, player, position, velocity, rotation] : query)
    {
        const float force = settings->getDouble("force", 5000.0F);
        const float jumpForce = settings->getDouble("jumpForce", 2000.0F);
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

        glm::vec3 targetTorso = {0.0F, 0.0F, 0.0F};
        glm::vec3 targetLeftHand = {7.0F, -3.0F, 0.0F};
        glm::vec3 targetRightHand = {-7.0F, -3.0F, 0.0F};
        glm::vec3 targetLeftFoot = {3.5F, -10.0F, 1.0F};
        glm::vec3 targetRightFoot = {-3.5F, -10.0F, 1.0F};

        if (player->isOnGround)
        {
            if (input->pressed("jump", player->id))
            {
                velocity->impulse += glm::vec3{0.0F, 1.0F, 0.0F} * jumpForce;
                player->isOnGround = false;
            }

            player->animationTime +=
                glm::sign(moveForce) * deltaTime->value * settings->getDouble("animationSpeed", 20.0F);

            if (moveForce != 0.0F)
            {
                targetLeftHand.y += glm::cos(player->animationTime);
                targetLeftHand.z += glm::sin(player->animationTime);
                targetRightHand.y += glm::cos(player->animationTime + glm::pi<float>() / 2.0F);
                targetRightHand.z += glm::sin(player->animationTime + glm::pi<float>() / 2.0F);

                targetLeftFoot.y += glm::cos(player->animationTime) * 0.5 + 0.5;
                targetLeftFoot.z += glm::sin(player->animationTime);
                targetRightFoot.y += glm::sin(player->animationTime) * 0.5 + 0.5;
                targetRightFoot.z += glm::cos(player->animationTime);

                targetTorso.y += glm::sin(player->animationTime) * 0.5;
            }

            player->isOnGround = false;
        }
        else
        {
            targetLeftHand.y += 5.0F;
            targetRightHand.y += 5.0F;
        }

        auto [torso] = *offsets[player->torso];
        auto [leftHand] = *offsets[player->leftHand];
        auto [rightHand] = *offsets[player->rightHand];
        auto [leftFoot] = *offsets[player->leftFoot];
        auto [rightFoot] = *offsets[player->rightFoot];
        torso->vec = glm::mix(torso->vec, targetTorso, deltaTime->value * 10.0F);
        leftHand->vec = glm::mix(leftHand->vec, targetLeftHand, deltaTime->value * 10.0F);
        rightHand->vec = glm::mix(rightHand->vec, targetRightHand, deltaTime->value * 10.0F);
        leftFoot->vec = glm::mix(leftFoot->vec, targetLeftFoot, deltaTime->value * 10.0F);
        rightFoot->vec = glm::mix(rightFoot->vec, targetRightFoot, deltaTime->value * 10.0F);
    }
}

// handle dead

void demo::playersPlugin(Cubos& cubos)
{
    cubos.addComponent<Player>();
    cubos.system(move).tagged("player.move").tagged("cubos.physics.apply_forces").before("cubos.transform.update");
}
