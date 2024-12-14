#include "plugin.hpp"
#include "../holdable/plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../player/plugin.hpp"
#include "../interpolation/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Box)
{
    return cubos::core::ecs::TypeBuilder<Box>("airships::client::Box")
        .withField("airships::client::freeSlots", &Box::freeSlots)
        .withField("airships::client::type", &Box::type)
        .build();
}

static int getTopOfStack(Entity entity,
                         Query<Entity, const airships::client::Holdable&, const Position&, const ChildOf&> heldObjects,
                         Entity& topEntity, Position& topPos)
{
    topEntity = entity;

    int topHeight = 0;
    while (auto match = heldObjects.pin(1, topEntity).first())
    {
        auto [next, holdable, pos, childOf] = *match;
        topPos = pos;
        topEntity = next;
        topHeight++;
    }
    return topHeight;
}

void airships::client::boxPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(airships::client::holdablePlugin);
    cubos.depends(airships::client::interactablePlugin);
    cubos.depends(airships::client::playerPlugin);
    cubos.depends(airships::client::interpolationPlugin);

    cubos.component<airships::client::Box>();

    cubos.observer("add and remove items upon Box interaction")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<Entity, Box&, Interaction&> boxes,
                 Query<Entity, const Holdable&, const Position&, const ChildOf&> held,
                 Query<Entity, InterpolationOf&, Player&> players, Query<Entity, InterpolationOf&> boxInterpolation) {
            for (auto [boxEnt, box, interaction] : boxes)
            {
                CUBOS_DEBUG("BOX INTERACTION");
                cmds.remove<Interaction>(boxEnt);

                auto [boxInterpolatedEnt, boxInterpolationOf] = *boxInterpolation.pin(1, boxEnt).first();

                // Get the interpolated entity of the player with interaction (we'll still call it player entity)
                auto [playerEnt, interpolationOf, player] = *players.pin(1, interaction.player).first();

                // If player is holding an object, add it to the box
                if (auto heldByPlayer = held.pin(1, playerEnt).first())
                {
                    CUBOS_DEBUG("Player is holding object.");
                    auto [heldEnt, holdable, pos, heldChildOf] = *heldByPlayer;
                    // If there are no freeSlots on the box, or box is of different type do nothing
                    if (box.freeSlots.empty() || box.type != holdable.type)
                    {
                        continue;
                    }

                    // Move held entity to free slot in box
                    Position slotPosition = box.freeSlots.back();
                    box.freeSlots.pop_back();
                    cmds.relate(heldEnt, boxInterpolatedEnt, ChildOf{}).add(heldEnt, slotPosition);
                    CUBOS_DEBUG("Putting item in slot {}", slotPosition);
                }
                else
                {
                    CUBOS_DEBUG("Player is NOT holding object.");
                    // If box is not empty (stack height not 1)
                    if (auto heldByBox = held.pin(1, boxInterpolatedEnt).first())
                    {
                        auto [heldEnt, holdable, pos, heldChildOf] = *heldByBox;

                        // Free the slot
                        box.freeSlots.push_back(pos);

                        cmds.relate(heldEnt, playerEnt, ChildOf{}).add(heldEnt, player.holdablePos);
                    }
                }
            }
        });
}