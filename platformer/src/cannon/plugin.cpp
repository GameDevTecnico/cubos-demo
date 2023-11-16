#include "plugin.hpp"
#include "cannon.hpp"
#include "bullet.hpp"

#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/transform/position.hpp>

using cubos::core::ecs::Commands;
using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void cannonSystem(Commands cmds, Query<Write<Cannon>, Read<Position>> cannons, Read<DeltaTime> deltaTime,
                         Write<Assets> assets)
{
    for (auto [entity, cannon, position] : cannons)
    {
        cannon->timer -= deltaTime->value;
        if (cannon->timer < 0.0F)
        {
            cmds.spawn(assets->read(cannon->bullet)->blueprint)
                .add("base", Position{position->vec})
                .get<Bullet>("base")
                .velocity = glm::vec3{0.0F, 0.0F, 15.0F};
            cannon->timer = cannon->bulletTime;
        }
    }
}

static void bulletSystem(Commands cmds, Query<Write<Bullet>, Write<Position>> bullets, Read<DeltaTime> deltaTime)
{
    for (auto [entity, bullet, position] : bullets)
    {
        position->vec += bullet->velocity * deltaTime->value;
        bullet->life -= deltaTime->value;
        if (bullet->life < 0.0F)
        {
            cmds.destroy(entity);
        }
    }
}

void demo::cannonPlugin(Cubos& cubos)
{
    cubos.addComponent<Cannon>();
    cubos.addComponent<Bullet>();
    cubos.system(cannonSystem);
    cubos.system(bulletSystem);
}
