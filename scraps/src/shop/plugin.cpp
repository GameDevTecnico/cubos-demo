#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"
#include "../holdable/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Coin)
{
    return cubos::core::ecs::TypeBuilder<Coin>("demo::Coin").build();
}

CUBOS_REFLECT_IMPL(demo::Shop)
{
    return cubos::core::ecs::TypeBuilder<Shop>("demo::Shop")
        .withField("item", &Shop::item)
        .withField("root", &Shop::root)
        .build();
}

void demo::shopPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(holdablePlugin);

    cubos.component<Coin>();
    cubos.component<Shop>();

    cubos.observer("handle interactions with shops")
        .onAdd<Interaction>()
        .call([](Commands cmds, Assets& assets, Query<const Interaction&, const Shop&> shopInteractions,
                 Query<Entity, const Coin&, const ChildOf&> coins) {
            for (auto [interaction, shop] : shopInteractions)
            {
                if (auto coinMatch = coins.pin(1, interaction.entity).first())
                {
                    auto [coinEnt, _1, _2] = coinMatch.value();

                    // Destroy the coin and spawn an item on the player's hand.
                    cmds.destroy(coinEnt);
                    auto itemEnt = cmds.spawn(assets.read(shop.item)->blueprint).entity(shop.root);
                    cmds.relate(itemEnt, interaction.entity, ChildOf{});
                    cmds.add(itemEnt, Position{{0.0F, 8.0F, 0.0F}});
                }
            }
        });
}
