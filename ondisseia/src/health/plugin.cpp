#include "plugin.hpp"
#include "../destroy_tree/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_EXTERNAL_IMPL(demo::Team)
{
    using cubos::core::reflection::EnumTrait;
    using cubos::core::reflection::Type;

    return Type::create("demo::Team")
        .with(EnumTrait{}
                  .withVariant<demo::Team::PLAYER_1>("Player 1")
                  .withVariant<demo::Team::PLAYER_2>("Player 2")
                  .withVariant<demo::Team::PLAYER_3>("Player 3")
                  .withVariant<demo::Team::PLAYER_4>("Player 4"));
}

CUBOS_REFLECT_IMPL(demo::Damage)
{
    return cubos::core::ecs::TypeBuilder<Damage>("demo::Damage").withField("hp", &Damage::hp).build();
}

CUBOS_REFLECT_IMPL(demo::Health)
{
    return cubos::core::ecs::TypeBuilder<Health>("demo::Health")
        .withField("hp", &Health::hp)
        .withField("team", &Health::team)
        .build();
}

void demo::healthPlugin(Cubos& cubos)
{
    cubos.depends(destroyTreePlugin);

    cubos.component<Damage>();
    cubos.component<Health>();

    cubos.observer("apply damage")
        .onAdd<Damage>()
        .call([](Commands cmds, Query<Entity, const Damage&, Opt<Health&>> query) {
            for (auto [ent, dmg, health] : query)
            {
                if (health.contains())
                {
                    health->hp -= dmg.hp;
                }

                cmds.remove<Damage>(ent);
            }
        });
}