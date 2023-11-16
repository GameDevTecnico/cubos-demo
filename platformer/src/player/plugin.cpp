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
#include "../mover/mover.hpp"
#include "../cannon/bullet.hpp"

using cubos::core::ecs::Commands;
using cubos::core::ecs::Entity;
using cubos::core::ecs::EventReader;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void move(Query<Write<Player>, Write<Position>, Write<PhysicsVelocity>, Write<Rotation>> query,
                 Query<Read<Mover>> movers, Query<Write<Offset>> offsets, Read<Input> input, Read<DeltaTime> deltaTime,
                 Write<Settings> settings, EventReader<CollisionEvent> collisions, Query<Read<Bullet>> bullets)
{
    glm::vec3 addVelocity[] = {{0.0F, 0.0F, 0.0F}, {0.0F, 0.0F, 0.0F}};

    for (auto collision : collisions)
    {
        // Only handle collisions between players and other entities which are not players.
        if (!query[collision.entity] /* || query[collision.other]*/)
        {
            continue;
        }

        auto [player, position, velocity, rotation] = *query[collision.entity];

        if (bullets[collision.other])
        {
            auto [bullet] = *bullets[collision.other];
            velocity->velocity += bullet->velocity * 10.0F;
            velocity->velocity.y += 10.0F;
            player->isOnGround = false;
            continue;
        }

        position->vec -= collision.normal * collision.penetration;
        velocity->velocity -= collision.normal * glm::dot(collision.normal, velocity->velocity);

        if (movers[collision.other])
        {
            auto [mover] = *movers[collision.other];
            CUBOS_DEBUG("deslocation: {}, {}, {}", mover->velocity.x, mover->velocity.y, mover->velocity.z);
            addVelocity[player->id] = mover->velocity;
        }

        if (collision.normal.y < -0.1F)
        {
            player->isOnGround = true;
        }
    }

    for (auto [entity, player, position, velocity, rotation] : query)
    {
        auto moveVertical = input->axis("vertical", player->id);
        if (glm::abs(moveVertical) < 0.1F)
        {
            moveVertical = 0.0F;
        }

        auto moveHorizontal = input->axis("horizontal", player->id);
        if (glm::abs(moveHorizontal) < 0.1F)
        {
            moveHorizontal = 0.0F;
        }

        auto jump = input->pressed("jump", player->id);

        glm::vec3 move =
            moveVertical * player->forward * player->speed - moveHorizontal * player->right * player->speed;

        if (player->isOnGround)
        {
            velocity->velocity.x = move.x;
            velocity->velocity.z = move.z;
            velocity->velocity += addVelocity[player->id];

            if (jump)
            {
                velocity->impulse += glm::vec3{0.0F, 1.0F, 0.0F} * player->jumpForce;
                player->isOnGround = false;
            }
        }

        glm::vec3 targetTorso = {0.0F, 0.0F, 0.0F};
        glm::vec3 targetLeftHand = {7.0F, -3.0F, 2.0F};
        glm::vec3 targetRightHand = {-7.0F, -3.0F, 2.0F};
        glm::vec3 targetLeftFoot = {3.5F, -10.0F, 1.0F};
        glm::vec3 targetRightFoot = {-3.5F, -10.0F, 1.0F};

        if (player->isOnGround)
        {
            player->animationTime += deltaTime->value * player->speed * player->animationSpeed;

            if (moveVertical != 0.0F || moveHorizontal != 0.0F)
            {
                targetLeftHand.y += glm::cos(player->animationTime);
                targetLeftHand.z += glm::sin(player->animationTime);
                targetRightHand.y += glm::cos(player->animationTime + glm::pi<float>() / 2.0F);
                targetRightHand.z += glm::sin(player->animationTime + glm::pi<float>() / 2.0F);

                targetLeftFoot.y += glm::cos(player->animationTime) * 0.5F + 0.5F;
                targetLeftFoot.z += glm::sin(player->animationTime);
                targetRightFoot.y += glm::cos(player->animationTime + glm::pi<float>()) * 0.5F + 0.5F;
                targetRightFoot.z += glm::sin(player->animationTime + glm::pi<float>());

                targetTorso.y += glm::sin(player->animationTime) * 0.5F;
            }

            player->isOnGround = false;
        }
        else
        {
            targetLeftHand.y += 5.0F;
            targetRightHand.y += 5.0F;
        }

        if (move.x != 0.0F || move.z != 0.0F)
        {
            auto desired = glm::quatLookAt(-glm::normalize(move), glm::vec3{0.0F, 1.0F, 0.0F});
            rotation->quat = glm::slerp(rotation->quat, desired, player->rotationSpeed * deltaTime->value);
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
