#include "plugin.hpp"
#include "mover.hpp"

#include <cubos/engine/transform/position.hpp>
#include <cubos/engine/physics/components/physics_velocity.hpp>

#include <glm/gtc/matrix_transform.hpp>

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;

using namespace cubos::engine;

using namespace demo;

static void moverSystem(Query<Write<Mover>, Write<Position>, Write<PhysicsVelocity>> movers, Read<DeltaTime> deltaTime)
{
    for (auto [entity, mover, position, velocity] : movers)
    {
        mover->time += deltaTime->value * (mover->direction ? 1.0F : -1.0F);
        if (mover->time > mover->duration || mover->time < 0.0F)
        {
            mover->time = glm::clamp(mover->time, 0.0F, mover->duration);
            mover->direction = !mover->direction;
        }

        auto prev = position->vec;
        position->vec = glm::mix(mover->from, mover->to, mover->time / mover->duration);
        velocity->velocity = (position->vec - prev) / deltaTime->value;
    }
}

void demo::moverPlugin(Cubos& cubos)
{
    cubos.addComponent<Mover>();
    cubos.system(moverSystem).before("cubos.transform.update");
}
