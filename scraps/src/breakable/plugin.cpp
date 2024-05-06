#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"

#include <random>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Breakable)
{
    return cubos::core::ecs::TypeBuilder<Breakable>("demo::Breakable")
        .withField("drop", &Breakable::drop)
        .withField("root", &Breakable::root)
        .withField("minDrops", &Breakable::minDrops)
        .withField("maxDrops", &Breakable::maxDrops)
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
                CUBOS_ASSERT(breakable.maxDrops >= breakable.minDrops);

                cmds.destroy(ent);

                int dropCount = rand() % (breakable.maxDrops - breakable.minDrops + 1) + breakable.minDrops;
                for (int i = 0; i < dropCount; ++i)
                {
                    auto dropEnt = cmds.spawn(assets.read(breakable.drop)->blueprint).entity(breakable.root);
                    cmds.relate(dropEnt, parentEnt, ChildOf{});
                    cmds.add(dropEnt, Object{.position = object.position + object.size / 2, .size = {1, 1}});
                }
            }
        });
}
