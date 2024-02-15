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
        .before("cubos.transform.update")
        .without<Dead>() // Don't move dead cars.
        .call([](Commands cmds, const DeltaTime& dt, const Input& input, Query<Car&, Position&, Rotation&> query) {
            for (auto [car, position, rotation] : query)
            {
                // Get the user's input.
                auto throttle = glm::clamp(input.axis("throttle", car.player), 0.0F, 1.0F);
                auto brake = glm::clamp(input.axis("brake", car.player), 0.0F, 1.0F);
                auto steer = -input.axis("steer", car.player);

                // Get the car's directional vectors.
                auto forward = rotation.quat * glm::vec3(0.0F, 0.0F, 1.0F);
                auto right = rotation.quat * glm::vec3(1.0F, 0.0F, 0.0F);

                // Turn the car's wheels.
                auto maxSteering = car.steeringSpeed * dt.value;
                auto wheelAngleDelta = steer * car.maxWheelAngle - car.wheelAngle;
                car.wheelAngle += glm::clamp(wheelAngleDelta, -maxSteering, maxSteering);

                // Add acceleration the car's velocity.
                auto acceleration = throttle * car.acceleration;
                car.linearVelocity += acceleration * forward * dt.value;

                // Limit the car's velocity.
                auto forwardVelocity = glm::dot(car.linearVelocity, forward);
                if (forwardVelocity > car.maxVelocity)
                {
                    car.linearVelocity -= forward * (forwardVelocity - car.maxVelocity);
                    forwardVelocity = car.maxVelocity;
                }

                // Apply drag and braking to the car's velocity.
                if (forwardVelocity != 0.0F)
                {
                    auto drag = car.forwardDrag + car.forwardDragCoefficient * glm::abs(forwardVelocity);
                    drag += brake * car.braking;
                    drag *= dt.value;
                    drag = glm::min(drag, glm::abs(forwardVelocity));
                    car.linearVelocity -= forward * drag * glm::sign(forwardVelocity);
                }

                auto lateralVelocity = glm::dot(car.linearVelocity, right);
                if (lateralVelocity != 0.0F)
                {
                    auto drag = car.lateralDrag + car.lateralDragCoefficient * glm::abs(lateralVelocity);
                    drag *= dt.value;
                    drag = glm::min(drag, glm::abs(lateralVelocity));
                    car.linearVelocity -= right * drag * glm::sign(lateralVelocity);
                }

                auto grip = car.wheelGrip;

                // Calculate the centripetal acceleration necessary for the car to follow the curve.
                forwardVelocity = glm::dot(car.linearVelocity, forward);
                auto turningRadius = car.wheelBase / glm::sin(glm::radians(glm::abs(car.wheelAngle)));
                auto turningAngularVelocity = glm::sign(car.wheelAngle) * forwardVelocity / turningRadius;
                auto centripetalAcceleration = turningAngularVelocity * turningAngularVelocity / turningRadius;

                // Calculate how much of the car's velocity can be changed to follow the curve.
                auto turningRate = 1.0F;
                if (centripetalAcceleration > 0.0F)
                {
                    turningRate = glm::min(1.0F, grip / glm::abs(centripetalAcceleration));
                }

                // Calculate the angular velocity necessary for the car to follow the curve.
                if (centripetalAcceleration > 0.0F)
                {
                    car.angularVelocity = turningAngularVelocity;
                }
                else if (turningRate > 0.0F)
                {
                    car.angularVelocity = 0.0F;
                }

                // Update the car's rotation.
                auto rotationDelta = glm::angleAxis(car.angularVelocity * dt.value, glm::vec3(0.0F, 1.0F, 0.0F));
                rotation.quat = rotationDelta * rotation.quat;

                // Rotate the car's velocity to match the car's new orientation.
                car.linearVelocity =
                    car.linearVelocity * (1.0F - turningRate) + rotationDelta * car.linearVelocity * turningRate;

                // Update the car's position.
                position.vec += car.linearVelocity * dt.value;
            }
        });

    cubos.system("freeze car").with<Dead>().call([](Query<Car&> query) {
        for (auto [car] : query)
        {
            car.linearVelocity = glm::vec3(0.0F);
            car.angularVelocity = 0.0F;
            car.wheelAngle = 0.0F;
        }
    });
}
