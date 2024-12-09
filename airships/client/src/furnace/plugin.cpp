#include "plugin.hpp"
#include "../drivable/plugin.hpp"
#include "../interactable/plugin.hpp"

#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::Furnace)
{
    return cubos::core::ecs::TypeBuilder<Furnace>("airships::client::Furnace")
        .withField("coal", &Furnace::coal)
        .withField("addCoal", &Furnace::addCoal)
        .withField("maximumCoal", &Furnace::maximumCoal)
        .withField("maxUpVelocity", &Furnace::maxUpVelocity)
        .withField("maxVelocityDown", &Furnace::maxVelocityDown)
        .build();
}

static float map(float x, float inMin, float inMax, float outMin, float outMax)
{
    return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
}

void airships::client::furnacePlugin(cubos::engine::Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(drivablePlugin);
    cubos.depends(interactablePlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(fixedStepPlugin);

    cubos.component<Furnace>();

    cubos.observer("add Interactable to Furnace").onAdd<Furnace>().call([](Commands cmds, Query<Entity> query) {
        for (auto [entity] : query)
        {
            cmds.add(entity, Interactable{});
        }
    });

    cubos.observer("handle Furnace interaction")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<Entity, Furnace&, Interaction&> query) {
            for (auto [entity, furnace, interaction] : query)
            {
                cmds.remove<Interaction>(entity);
                furnace.coal = glm::min(furnace.coal + furnace.addCoal, furnace.maximumCoal);
            }
        });

    cubos.system("update Furnace entities")
        .tagged(physicsApplyForcesTag)
        .call([](const DeltaTime& dt, Query<Furnace&, const ChildOf&, Drivable&, Mass&, Velocity&, Force&> query) {
            for (auto [furnace, childOf, drivable, mass, velocity, force] : query)
            {
                // Update vertical velocity
                if (furnace.coal >= furnace.addCoal)
                {
                    velocity.vec.y = map(furnace.coal, 0.0F, furnace.maximumCoal, 0, furnace.maxUpVelocity);
                }
                else
                {
                    velocity.vec.y = map(furnace.coal, 0.0F, furnace.addCoal, furnace.maxVelocityDown, 0.0F);
                }

                furnace.coal = glm::max(furnace.coal - dt.value(), 0.0F);
            }
        });

    // cubos.system("add gravity")
}