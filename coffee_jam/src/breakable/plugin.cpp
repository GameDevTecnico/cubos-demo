#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Breakable)
{
    return cubos::core::ecs::TypeBuilder<Breakable>("demo::Breakable")
        .withField("drop", &Breakable::drop)
        .withField("root", &Breakable::root)
        .build();
}

void demo::breakablePlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(objectPlugin);

    cubos.component<Breakable>();

    cubos.observer("break Breakable when interacted with")
        .onAdd<Interaction>()
        .call([](Commands cmds, Assets& assets,
                 Query<Entity, const Object&, const Breakable&, const ChildOf&, Entity> query) {
            for (auto [ent, object, breakable, childOf, parentEnt] : query)
            {
                cmds.destroy(ent);
                auto dropEnt = cmds.spawn(assets.read(breakable.drop)->blueprint).entity(breakable.root);
                cmds.relate(dropEnt, parentEnt, ChildOf{});
                cmds.add(dropEnt, Object{.position = object.position, .size = object.size});
            }
        });
}
