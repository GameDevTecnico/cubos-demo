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
#include <cubos/engine/fixed_step/plugin.hpp>
#include <map>
#include <unordered_set>

#include "plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../follow/plugin.hpp"
#include "../animation/plugin.hpp"
#include "../interpolation/plugin.hpp"
#include "../player_id/plugin.hpp"

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Player)
{
    return cubos::core::ecs::TypeBuilder<Player>("airships::client::Player")
        .withField("horizontalAxis", &Player::horizontalAxis)
        .withField("verticalAxis", &Player::verticalAxis)
        .withField("interactAction", &Player::interactAction)
        .withField("holdablePos", &Player::holdablePos)
        .withField("interactDistance", &Player::interactDistance)
        .withField("interactingWith", &Player::interactingWith)
        .withField("direction", &Player::direction)
        .withField("moveSpeed", &Player::moveSpeed)
        .withField("halfRotationTime", &Player::halfRotationTime)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::PlayerSkin)
{
    return cubos::core::ecs::TypeBuilder<PlayerSkin>("airships::client::PlayerSkin")
        .withField("idleAnimation", &PlayerSkin::idleAnimation)
        .withField("walkAnimation", &PlayerSkin::walkAnimation)
        .build();
}

void airships::client::playerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(renderVoxelsPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(followPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(animationPlugin);
    cubos.depends(interpolationPlugin);
    cubos.depends(playerIdPlugin);

    cubos.component<Player>();
    cubos.component<PlayerSkin>();

    cubos.system("initialize Player position properly")
        .before(transformUpdateTag)
        .call([](Query<Player&, Position&> query) {
            for (auto [player, pos] : query)
            {
                if (!player.initialized)
                {
                    pos.vec = {0.0F, 0.0F, 0.0F};
                    player.initialized = true;
                }
            }
        });

    cubos.system("do Player movement")
        .tagged(fixedStepTag)
        .before(collisionsTag)
        .call([](Commands cmds,
                 Query<LocalToWorld&, const Follow&, RenderAnimation&, const InterpolationOf&, Player&,
                       const PlayerSkin&, const PlayerId&, Position&, Rotation&, const ChildOf&, const LocalToWorld&>
                     players,
                 const FixedDeltaTime& dt, const Input& input) {
            for (auto [cameraLTW, follow, animation, interpolationOf, player, skin, playerId, pos, rot, childOf,
                       boatLTW] : players)
            {
                if (playerId.id == -1 || !player.interactingWith.isNull())
                {
                    animation.play(skin.idleAnimation);
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
                moveDirection.x = input.axis(player.horizontalAxis.c_str(), playerId.id);
                moveDirection.z = -input.axis(player.verticalAxis.c_str(), playerId.id);
                moveDirection = moveDirection.x * right + moveDirection.z * forward;
                if (glm::length2(moveDirection) == 0.0F)
                {
                    animation.play(skin.idleAnimation);
                    continue;
                }
                animation.play(skin.walkAnimation);
                moveDirection = glm::normalize(moveDirection);

                // Update the player position
                pos.vec += moveDirection * player.moveSpeed * dt.value;

                // Slowly rotate the player to the movement direction
                auto targetRotation = glm::quatLookAt(-moveDirection, glm::vec3{0.0F, 1.0F, 0.0F});
                float rotationAlpha = 1.0F - glm::pow(0.5F, dt.value / player.halfRotationTime);
                rot.quat = glm::slerp(rot.quat, targetRotation, rotationAlpha);
            }
        });

    cubos.system("do Player interaction")
        .call([](Commands cmds, Query<Entity, Player&, const PlayerId&, const LocalToWorld&> players,
                 Query<const Interactable&> interactables, const Assets& assets, Raycast raycast, const Input& input) {
            for (auto [playerEnt, player, playerId, playerLTW] : players)
            {
                if (input.justPressed(player.interactAction.c_str(), playerId.id))
                {
                    if (player.interactingWith.isNull())
                    {
                        Raycast::Ray ray{.origin = playerLTW.worldPosition(), .direction = playerLTW.forward()};
                        ray.mask = 1 << 1; // Only hit interactables
                        if (auto hit = raycast.fire(ray);
                            hit && glm::distance(hit->point, ray.origin) < player.interactDistance)
                        {
                            if (interactables.at(hit->entity))
                            {
                                cmds.add(hit->entity, Interaction{.player = playerEnt});
                            }
                        }
                    }
                    else
                    {
                        cmds.add(player.interactingWith, Interaction{.player = playerEnt});
                    }
                }
            }
        });

    cubos.system("handle Player collisions")
        .tagged(fixedStepTag)
        .after(collisionsTag)
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

                for (auto& manifold : collidingWith.manifolds)
                {
                    // Get the normal of the collision.
                    auto normal = manifold.normal;
                    if (collidingWith.entity != ent)
                    {
                        normal = -normal;
                    }

                    // Transform the normal into the parent space.
                    normal = glm::normalize(glm::vec3(glm::inverse(parentLTW) * glm::vec4(normal, 0.0F)));

                    // Calculate the necessary offset to separate the player from the collider.
                    float penetration = 0.0F;
                    for (auto& contact : manifold.points)
                    {
                        penetration = std::max(penetration, contact.penetration);
                    }
                    pos.vec -= normal * penetration;
                }
            }
        });
}