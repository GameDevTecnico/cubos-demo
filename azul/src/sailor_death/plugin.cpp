#include "plugin.hpp"
#include "../destroy_tree/plugin.hpp"
#include "../health/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::SailorDeath)
{
    return cubos::core::ecs::TypeBuilder<SailorDeath>("demo::SailorDeath").withField("hp", &SailorDeath::hp).build();
}

void demo::sailorDeathPlugin(Cubos& cubos)
{
    cubos.depends(healthPlugin);
    cubos.depends(destroyTreePlugin);

    cubos.relation<SailorDeath>();

    cubos.system("kill sailors").call([](Commands cmds, Query<Entity, const SailorDeath&, const Health&> query) {
        for (auto [ent, death, health] : query)
        {
            if (health.hp <= death.hp)
            {
                cmds.add(ent, DestroyTree{});
            }
        }
    });
}
