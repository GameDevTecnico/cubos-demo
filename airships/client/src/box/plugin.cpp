#include "plugin.hpp"
#include "../holdable/plugin.hpp"
#include "../interactable/plugin.hpp"
#include "../player/plugin.hpp"
#include "../interpolation/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/vector.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/render/voxels/plugin.hpp>
#include <cubos/engine/render/voxels/grid.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Box)
{
    return cubos::core::ecs::TypeBuilder<Box>("airships::client::Box")
        .withField("airships::client::freeSlots", &Box::freeSlots)
        .withField("airships::client::type", &Box::type)
        .build();
}

void airships::client::boxPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(airships::client::holdablePlugin);
    cubos.depends(airships::client::interactablePlugin);
    cubos.depends(airships::client::playerPlugin);
    cubos.depends(airships::client::interpolationPlugin);
    cubos.depends(renderVoxelsPlugin);

    cubos.component<airships::client::Box>();

    cubos.observer("add Interactable to Box").onAdd<Box>().call([](Commands cmds, Query<Entity> query) {
        for (auto [entity] : query)
        {
            cmds.add(entity, Interactable{});
        }
    });

    cubos.observer("add and remove items upon Box interaction")
        .onAdd<Interaction>(1)
        .call([](Commands cmds, Query<Entity, InterpolationOf&, Entity, Box&, Interaction&> boxes,
                 Query<Entity, const Holdable&, const RenderVoxelGrid&, const Position&, const Scale&, const ChildOf&>
                     held,
                 Query<Entity, InterpolationOf&, Player&> players) {
            for (auto [boxInterpolatedEnt, boxInterpolationOf, boxEnt, box, interaction] : boxes)
            {
                cmds.remove<Interaction>(boxEnt);

                // Get the interpolated entity of the player with interaction (we'll still call it player entity)
                auto [playerEnt, interpolationOf, player] = *players.pin(1, interaction.player).first();

                // If player is holding an object, add it to the box
                if (auto heldByPlayer = held.pin(1, playerEnt).first())
                {
                    auto [heldEnt, holdable, voxel, pos, scale, heldChildOf] = *heldByPlayer;
                    // If there are no freeSlots on the box, or box is of different type do nothing
                    if (box.freeSlots.empty() || box.type != holdable.type)
                    {
                        cmds.destroy(heldEnt);
                        continue;
                    }

                    // Move held entity to free slot in box
                    Position slotPosition = box.freeSlots.back();
                    box.freeSlots.pop_back();
                    cmds.relate(heldEnt, boxInterpolatedEnt, ChildOf{}).add(heldEnt, slotPosition);
                }
                else
                {
                    // If box is not empty, copy item
                    if (auto heldByBox = held.pin(1, boxInterpolatedEnt).first())
                    {
                        auto [heldEnt, holdable, voxel, pos, scale, heldChildOf] = *heldByBox;

                        heldEnt = cmds.create().add(holdable).add(voxel).add(pos).add(scale).entity();

                        // Free the slot
                        // box.freeSlots.push_back(pos);

                        cmds.relate(heldEnt, playerEnt, ChildOf{}).add(heldEnt, player.holdablePos);
                    }
                }
            }
        });
}