#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::AutoChildOf)
{
    return cubos::core::ecs::TypeBuilder<AutoChildOf>("airships::client::AutoChildOf")
        .withField("depth", &AutoChildOf::depth)
        .tree()
        .build();
}

void airships::client::autoChildOfPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);

    cubos.relation<AutoChildOf>();

    cubos.system("auto add DrawsTo to render targets")
        .call([](Commands cmds, Query<Entity, const AutoChildOf&, Entity> query, Query<const ChildOf&, Entity> childOfQuery) {
            for (auto [child, autoChildOf, originalParent] : query)
            {
                Entity parent = originalParent;
                for (int i = 0; i < autoChildOf.depth; ++i)
                {
                    if (auto match = childOfQuery.pin(0, parent).first())
                    {
                        auto [childOf, entity] = *match;
                        parent = entity;
                    }
                    else
                    {
                        parent = {};
                        break;
                    }
                }
                
                if (!parent.isNull())
                {
                    cmds.relate(child, parent, ChildOf{});
                    cmds.unrelate<AutoChildOf>(child, originalParent);
                }
            }
        });
}
