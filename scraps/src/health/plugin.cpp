#include "plugin.hpp"
#include "cubos/engine/prelude.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Health)
{
    return cubos::core::ecs::TypeBuilder<Health>("demo::Health")
        .withField("hp", &Health::hp)
        .withField("team", &Health::team)
        .build();
}

void demo::healthPlugin(Cubos& cubos)
{

    cubos.component<Health>();

    cubos.system("check healthbars").call([](Commands cmds, Query<Entity, const Health&> query) {
        for (auto [entity, hp] : query)
        {
            if (hp.hp <= 0)
            {
                cmds.destroy(entity);
            }
        }
    });
}