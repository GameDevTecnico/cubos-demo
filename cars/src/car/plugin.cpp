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
                // When the user provides no input, the wheel's velocity will be set to the car's velocity.
                // When the user breaks, the wheel's velocity will be set to a number lower than  the car's velocity.
                // When the user accelerates, the wheel's velocity will be set to a number greater than the car's
                // velocity, up to a maximum value.
                //
                // If the difference between the car's velocity and the wheel's velocity is higher than the wheels'
                // grip, the car will skid. In this case, the car's angular velocity will be kept constant, until the
                // car's speed is low enough to allow the wheels to grip again.
                //
                // Otherwise the car's angular velocity will be modified based on the car's speed and the user's input.

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

                // Update wheels' velocity.
                auto wheelAcceleration = throttle * car.acceleration;
                car.wheelVelocity += wheelAcceleration * dt.value;
                auto wheelBraking = brake * car.braking * dt.value;
                car.wheelVelocity -= glm::min(wheelBraking, car.wheelVelocity);
                car.wheelVelocity = glm::clamp(car.wheelVelocity, 0.0F, car.maxWheelVelocity);

                // Pass as much of the wheel's velocity into the the car's velocity as possible.
                auto remainingGrip = car.wheelGrip * dt.value;
                auto velocityDifference = car.wheelVelocity - glm::dot(car.linearVelocity, forward);
                velocityDifference = glm::clamp(velocityDifference, -2.0F * remainingGrip, 2.0F * remainingGrip);
                remainingGrip -= 0.5F * glm::abs(velocityDifference);
                car.linearVelocity += 0.5F * velocityDifference * forward;
                car.wheelVelocity -= 0.5F * velocityDifference;

                // Calculate the centripetal acceleration necessary for the car to follow the curve.
                auto turningLinearVelocity = glm::dot(car.linearVelocity, forward);
                auto turningRadius = car.wheelBase / glm::sin(glm::radians(glm::abs(car.wheelAngle)));
                auto turningAngularVelocity = glm::sign(car.wheelAngle) * turningLinearVelocity / turningRadius;
                auto centripetalAcceleration = turningAngularVelocity * turningAngularVelocity / turningRadius;

                // Calculate how much of the car's velocity can be changed to follow the curve.
                auto turningGrip = 1.0F;
                if (centripetalAcceleration > 0.0F)
                {
                    turningGrip = glm::min(1.0F, remainingGrip / glm::abs(centripetalAcceleration));
                }

                // Calculate the angular velocity necessary for the car to follow the curve.
                if (centripetalAcceleration > 0.0F)
                {
                    // Kill any lateral velocity.
                    car.angularVelocity = turningAngularVelocity;
                }
                else if (remainingGrip > 0.0F)
                {
                    car.angularVelocity = 0.0F;
                }

                // Update the car's rotation.
                auto rotationDelta = glm::angleAxis(car.angularVelocity * dt.value, glm::vec3(0.0F, 1.0F, 0.0F));
                rotation.quat = rotationDelta * rotation.quat;

                // Rotate the car's velocity to match the car's new orientation.
                car.linearVelocity = (rotationDelta * turningGrip) * car.linearVelocity;

                // Apply drag to the car's velocity.
                auto forwardVelocity = glm::dot(car.linearVelocity, forward);
                if (forwardVelocity != 0.0F)
                {
                    auto drag = car.forwardDrag + car.forwardDragCoefficient * glm::abs(forwardVelocity);
                    drag *= car.wheelGrip * dt.value;
                    drag = glm::min(drag, glm::abs(forwardVelocity));
                    car.linearVelocity -= forward * drag * glm::sign(forwardVelocity);
                }

                auto lateralVelocity = glm::dot(car.linearVelocity, right);
                if (lateralVelocity != 0.0F)
                {
                    auto drag = car.lateralDrag + car.lateralDragCoefficient * glm::abs(lateralVelocity);
                    drag *= car.wheelGrip * dt.value;
                    drag = glm::min(drag, glm::abs(lateralVelocity));
                    car.linearVelocity -= right * drag * glm::sign(lateralVelocity);
                }

                // Update the car's position.
                position.vec += car.linearVelocity * dt.value;
            }
        });

    cubos.system("freeze car").with<Dead>().call([](Query<Car&> query) {
        for (auto [car] : query)
        {
            car.linearVelocity = glm::vec3(0.0F);
            car.angularVelocity = 0.0F;
            car.wheelVelocity = 0.0F;
            car.wheelAngle = 0.0F;
        }
    });
}
