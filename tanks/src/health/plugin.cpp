#include "plugin.hpp"
#include "health.hpp"
#include "death.hpp"

using namespace cubos::engine;

void healthPlugin(Cubos& cubos)
{
    cubos.component<Health>();
    cubos.component<Death>();

    cubos.system("kill entities with non-positive health")
        .without<Death>() // We don't want to kill dead entities
        .call([](Commands cmds, Query<Entity, const Health&> query) {
            for (auto [entity, health] : query)
            {
                if (health.points <= 0)
                {
                    cmds.add(entity, Death{});
                }
            }
        });
}
