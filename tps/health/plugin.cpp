#include "health/plugin.hpp"

void healthPlugin(cubos::engine::Cubos& cubos)
{
    cubos.addComponent<Health>();
    cubos.addComponent<Death>();

    cubos.startupSystem("set player hp on start").call([](cubos::engine::Query<Health&> query) {
        //TODO: We might want a way to check if the entity is a player to set diff hp
        for (auto [health] : query)
        {
            health.points = 100;
        }
    });

    cubos.system("update player hp").call([](cubos::engine::Query<Entity, const Health&> query, cubos::engine::Commands& cmds){
        for (auto [entity, health] : query)
        {
            if (health.points <= 0)
            {
                cmds.add(entity, Death{})
            }
        }
    });
}
