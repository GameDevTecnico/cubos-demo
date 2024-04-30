#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Hammer)
{
    return cubos::core::ecs::TypeBuilder<Hammer>("demo::Hammer").build();
}

CUBOS_REFLECT_IMPL(demo::Ingredient)
{
    return cubos::core::ecs::TypeBuilder<Ingredient>("demo::Ingredient")
        .withField("result", &Ingredient::result)
        .withField("root", &Ingredient::root)
        .build();
}

static void destroyChildren(Commands& cmds, Entity ent, Query<Entity, const ChildOf&>& children)
{
    for (auto [childEnt, childOf] : children.pin(1, ent))
    {
        destroyChildren(cmds, childEnt, children);
        cmds.destroy(childEnt);
    }
}

void demo::hammerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(objectPlugin);

    cubos.component<Hammer>();
    cubos.component<Ingredient>();

    cubos.observer("swap Ingredient by result when interacted with Hammer")
        .onAdd<Interaction>()
        .call(
            [](Commands cmds, Assets& assets,
               Query<Entity, const Interaction&, const Object&, const Ingredient&, const ChildOf&, Entity> ingredients,
               Query<const Hammer&, const ChildOf&> hammers, Query<Entity, const ChildOf&> children) {
                for (auto [ent, interaction, object, ingredient, childOf, parentEnt] : ingredients)
                {
                    if (!hammers.pin(1, interaction.entity).empty())
                    {
                        cmds.destroy(ent);
                        destroyChildren(cmds, ent, children);

                        auto dropEnt = cmds.spawn(assets.read(ingredient.result)->blueprint).entity(ingredient.root);
                        cmds.relate(dropEnt, parentEnt, ChildOf{});
                        cmds.add(dropEnt, Object{.position = object.position, .size = object.size});
                    }
                }
            });
}
