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
        if (!mover->hasStarted)
        {
            position->vec = mover->from;
            mover->hasStarted = true;
            mover->direction = true;
        }

        auto source = mover->direction ? mover->from : mover->to;
        auto target = mover->direction ? mover->to : mover->from;

        if (glm::dot(target - position->vec, source - position->vec) > 0.0F)
        {
            mover->direction = !mover->direction;
            std::swap(source, target);
        }

        velocity->velocity = glm::normalize(target - position->vec) * mover->speed;
        velocity->force = {0.0F, 9.81F, 0.0F};
    }
}

void demo::moverPlugin(Cubos& cubos)
{
    cubos.addComponent<Mover>();
    cubos.system(moverSystem).after("cubos.physics.apply_forces");
}
