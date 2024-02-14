#include "plugin.hpp"
#include "car.hpp"
#include "../dead.hpp"

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/input.hpp>

using namespace cubos::engine;

void demo::carPlugin(Cubos& cubos)
{
    cubos.addComponent<Car>();

    cubos.system("move car")
        .without<Dead>() // Don't move dead cars.
        .call([](Commands cmds, const DeltaTime& dt, const Input& input, Query<Car&, Position&, Rotation&> query) {
            for (auto [car, position, rotation] : query)
            {
                // Get input from the player.
                auto acceleration = glm::clamp(input.axis("accelerate", car.player), 0.0F, 1.0F) * car.maxAcceleration;
                auto breaking = glm::clamp(input.axis("break", car.player), 0.0F, 1.0F) * car.maxBreaking;
                auto turn = -input.axis("turn", car.player) * car.maxTurn;

                // Modify turning speed based on the car's velocity.
                auto absoluteVelocity = glm::length(car.linearVelocity);
                if (absoluteVelocity < car.turnPeak)
                {
                    turn *= absoluteVelocity / car.turnPeak;
                }
                else
                {
                    turn *= 1.0F + glm::sqrt(absoluteVelocity - car.turnPeak) * car.turnDecay;
                }

                // Modify the car's angular velocity.
                car.angularVelocity += turn * dt.value;

                // Apply angular drag.
                auto angularDrag = car.angularVelocity * car.angularDragMul * dt.value;
                if (car.angularVelocity > 0.0F)
                {
                    car.angularVelocity = glm::max(0.0F, car.angularVelocity - angularDrag);
                }
                else
                {
                    car.angularVelocity = glm::min(0.0F, car.angularVelocity + angularDrag);
                }

                // Update the car's rotation.
                auto rotationDelta = glm::angleAxis(car.angularVelocity * dt.value, glm::vec3(0.0F, 1.0F, 0.0F));
                rotation.quat = rotationDelta * rotation.quat;

                // Calculate the car's forward and side vectors.
                auto forward = rotation.quat * glm::vec3(0.0F, 0.0F, 1.0F);
                auto side = rotation.quat * glm::vec3(1.0F, 0.0F, 0.0F);

                // Modify the car's linear velocity.
                car.linearVelocity =
                    car.linearVelocity * (1.0F - car.roadGrip) + rotationDelta * car.linearVelocity * car.roadGrip;
                car.linearVelocity += forward * acceleration * dt.value;

                // Calculate forward and side drag.
                auto forwardVelocity = glm::dot(car.linearVelocity, forward);
                auto sideVelocity = glm::dot(car.linearVelocity, side);
                auto forwardDrag =
                    glm::sign(forwardVelocity) *
                    glm::clamp((forwardVelocity * forwardVelocity * car.forwardDragMul + breaking) * dt.value, 0.0F,
                               glm::abs(forwardVelocity));
                auto sideDrag =
                    glm::sign(sideVelocity) *
                    glm::clamp(sideVelocity * sideVelocity * car.sideDragMul * dt.value, 0.0F, glm::abs(sideVelocity));

                // Apply forward and side drag.
                auto dragForce = forward * forwardDrag + side * sideDrag;
                car.linearVelocity -= dragForce;

                // Update the car's position.
                position.vec += car.linearVelocity * dt.value;
            }
        });

    CUBOS_TODO("handle Car vs Car collisions");
}
