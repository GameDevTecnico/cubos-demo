#include "plugin.hpp"
#include "cubos/engine/prelude.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/traits/enum.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/string.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_EXTERNAL_IMPL(demo::Team)
{
    using cubos::core::reflection::EnumTrait;
    using cubos::core::reflection::Type;

    return Type::create("demo::Team").with(
        EnumTrait{}
            .withVariant<demo::Team::PLAYER_1>("Player 1")
            .withVariant<demo::Team::PLAYER_2>("Player 2")
            .withVariant<demo::Team::PLAYER_3>("Player 3")
            .withVariant<demo::Team::PLAYER_4>("Player 4")
    );
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