#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>
#include <cubos/engine/render/voxels/load.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/collisions/colliding_with.hpp>
#include <glm/vec3.hpp>
#include <cubos/engine/collisions/raycast.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <map>

#include "plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../follow/plugin.hpp"
#include "../animation/plugin.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("airships::client::Player")
        .withField("player", &Player::player)
        .withField("horizontalAxis", &Player::horizontalAxis)
        .withField("verticalAxis", &Player::verticalAxis)
        .withField("interactAction", &Player::interactAction)
        .withField("idleAnimation", &Player::idleAnimation)
        .withField("walkAnimation", &Player::walkAnimation)
        .withField("canMove", &Player::canMove)
        .withField("direction", &Player::direction)
        .withField("moveSpeed", &Player::moveSpeed)
        .withField("halfRotationTime", &Player::halfRotationTime)
        .build();
}

void airships::client::playerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(followPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(animationPlugin);

    cubos.component<Player>();

    cubos.system("do Player movement")
        .call([](Commands cmds,
                 Query<LocalToWorld&, const Follow&, RenderAnimation&, Player&, Position&, Rotation&, const ChildOf&,
                       const LocalToWorld&>
                     players,
                 const DeltaTime& dt, const Input& input) {
            for (auto [cameraLTW, follow, animation, player, pos, rot, childOf, boatLTW] : players)
            {
                if (player.player == -1 || !player.canMove)
                {
                    animation.play(player.idleAnimation);
                    continue;
                }

                // Get the directional vectors from the camera's rotation.
                // We need to translate them to the player's parent space.
                auto forward4 = boatLTW.inverse() * cameraLTW.mat * glm::vec4{0.0F, 0.0F, -1.0F, 0.0F};
                glm::vec3 forward = {forward4.x, forward4.y, forward4.z};
                forward.y = 0.0F;
                forward = glm::normalize(forward);
                auto right = glm::cross(forward, {0.0F, 1.0F, 0.0F});

                // Get the movement vector from the directions above and the input
                glm::vec3 moveDirection = {0.0f, 0.0f, 0.0f};
                moveDirection.x = input.axis(player.horizontalAxis.c_str(), player.player);
                moveDirection.z = input.axis(player.verticalAxis.c_str(), player.player);
                moveDirection = moveDirection.x * right + moveDirection.z * forward;
                if (glm::length2(moveDirection) == 0.0F)
                {
                    animation.play(player.idleAnimation);
                    continue;
                }
                animation.play(player.walkAnimation);
                moveDirection = glm::normalize(moveDirection);

                // Update the player position
                pos.vec += moveDirection * player.moveSpeed * dt.value();

                // Slowly rotate the player to the movement direction
                auto targetRotation = glm::quatLookAt(-moveDirection, glm::vec3{0.0F, 1.0F, 0.0F});
                float rotationAlpha = 1.0F - glm::pow(0.5F, dt.value() / player.halfRotationTime);
                rot.quat = glm::slerp(rot.quat, targetRotation, rotationAlpha);
            }
        });

    cubos.system("do Player interaction")
        .call([](Commands cmds, Query<Entity, Player&, const LocalToWorld&> players,
                 Query<const Interactable&> interactables, const Assets& assets, Raycast raycast, const Input& input) {
            for (auto [playerEnt, player, playerLTW] : players)
            {
                if (input.justPressed(player.interactAction.c_str(), player.player))
                {
                    Raycast::Ray ray{.origin = playerLTW.worldPosition(), .direction = playerLTW.forward()};
                    ray.mask = 1 << 1; // Only hit interactables
                    if (auto hit = raycast.fire(ray))
                    {
                        if (interactables.at(hit->entity))
                        {
                            cmds.add(hit->entity, Interaction{.player = playerEnt});
                        }
                    }
                }
            }
        });

    cubos.system("handle Player collisions")
        .call([](Query<Entity, Player&, Position&, const CollidingWith&> query,
                 Query<const ChildOf&, const LocalToWorld&> parentQuery) {
            for (auto [ent, car, pos, collidingWith] : query)
            {
                glm::mat4 parentLTW = glm::mat4(1.0F);
                auto parent = parentQuery.pin(0, ent).first();
                if (parent)
                {
                    auto [childOf, ltw] = *parent;
                    parentLTW = ltw.mat;
                }

                // Get the normal of the collision.
                auto normal = collidingWith.normal;
                if (collidingWith.entity != ent)
                {
                    normal = -normal;
                }

                // Transform the normal into the parent space.
                normal = glm::normalize(glm::vec3(glm::inverse(parentLTW) * glm::vec4(normal, 0.0F)));

                // Calculate the necessary offset to separate the player from the collider.
                pos.vec -= normal * collidingWith.penetration;
            }
        });
}