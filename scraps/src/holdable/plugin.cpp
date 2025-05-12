#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Holdable)
{
    return cubos::core::ecs::TypeBuilder<Holdable>("demo::Holdable").wrap(&Holdable::stackable);
}

void demo::holdablePlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.component<Holdable>();

    cubos.observer("remove stacked entities when bottom is removed")
        .onRemove<Holdable>()
        .call([](Commands cmds, Query<Entity> query, Query<Entity, const ChildOf&> children) {
            for (auto [ent] : query)
            {
                for (auto [child, childOf] : children.pin(1, ent))
                {
                    cmds.destroy(child);
                }
            }
        });
}
