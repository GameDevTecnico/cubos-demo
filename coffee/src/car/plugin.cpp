#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/collisions/plugin.hpp>
#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/collisions/raycast.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/gizmos/plugin.hpp>
#include <cubos/engine/gizmos/target.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::Car)
{
    return cubos::core::ecs::TypeBuilder<Car>("coffee::Car")
        .withField("playerOwner", &Car::playerOwner)
        .withField("drivetrain", &Car::drivetrain)
        .withField("accelInput", &Car::accelInput)
        .withField("enginePower", &Car::enginePower)
        .withField("topSpeed", &Car::topSpeed)
        .withField("minimumSpeed", &Car::minimumSpeed)
        .withField("resistanceMassRatio", &Car::resistanceMassRatio)
        .withField("suspensionRestDist", &Car::suspensionRestDist)
        .withField("wheelRadius", &Car::wheelRadius)
        .withField("springStrength", &Car::springStrength)
        .withField("springDamper", &Car::springDamper)
        .withField("minSuspensionHeight", &Car::minSuspensionHeight)
        .withField("suspensionHalfTime", &Car::suspensionHalfTime)
        .withField("turnHalfTime", &Car::turnHalfTime)
        .build();
}

CUBOS_REFLECT_IMPL(coffee::Wheel)
{
    return cubos::core::ecs::TypeBuilder<Wheel>("coffee::Wheel")
        .withField("axis", &Wheel::axis)
        .withField("gripFactor", &Wheel::gripFactor)
        .withField("mass", &Wheel::mass)
        .withField("brakeForce", &Wheel::brakeForce)
        .build();
}

namespace coffee
{
    static void calculateGravity(const LocalToWorld& wheelLTW, const Mass& carMass, Force& carForce,
                                 const CenterOfMass& carCOM, const LocalToWorld& carLTW)
    {
        glm::vec3 gravitationForce = carMass.mass / 4.0F * glm::vec3(0.0F, -30.0F, 0.0F);
        carForce.addForceOnPoint(gravitationForce, wheelLTW.worldPosition() - carLTW.worldPosition(), carCOM.vec);
    }

    static void calculateSuspensionForces(const float distance, const LocalToWorld& wheelLTW, const Car& car,
                                          const Mass& carMass, Force& carForce, const Velocity& carVelocity,
                                          const AngularVelocity& carAngVelocity, const CenterOfMass& carCOM,
                                          const LocalToWorld& carLTW)
    {
        glm::vec3 springDir = wheelLTW.up();

        // world-space velocity of the tire
        glm::vec3 r1 = wheelLTW.worldPosition() - carLTW.worldPosition();
        glm::vec3 tireWorldVelocity = carVelocity.vec + glm::cross(carAngVelocity.vec, r1);

        // calculat offset from rest distance
        float offset = car.suspensionRestDist - distance;

        float springDirVelocity = glm::dot(springDir, tireWorldVelocity);
        float springStrength = car.springStrength * carMass.mass / 4.0F / car.wheelRadius;
        float force = (offset * springStrength) - (springDirVelocity * car.springDamper);

        carForce.addForceOnPoint(springDir * force, wheelLTW.worldPosition() - carLTW.worldPosition(), carCOM.vec);

        // gizmos.color({0.0F, 1.0F, 0.0F});
        // gizmos.drawLine("line", wheelLTW.worldPosition(), wheelLTW.worldPosition() + springDir * force);
    }

    static void calculateSteeringForces(const Wheel& wheel, const LocalToWorld& wheelLTW, const Car& car,
                                        const Mass& carMass, Force& carForce, Torque& carTorque,
                                        const Velocity& carVelocity, const AngularVelocity& carAngVelocity,
                                        const CenterOfMass& carCOM, const LocalToWorld& carLTW)
    {
        glm::vec3 steeringDir =
            glm::cross(wheelLTW.forward(), wheelLTW.up()); // we should be able to do wheelLTW.right()

        if (car.handBrakeOn && wheel.axis == 1 && car.accelInput > 0.0F)
        {
            float carLateralSpeed = glm::abs(glm::dot(steeringDir, carVelocity.vec));
            carTorque.add({0.0F,
                           carMass.mass * car.steerInput *
                               glm::lerp(70.0F, 0.0F, glm::clamp(carLateralSpeed / 30.0F, 0.0F, 1.0F)),
                           0.0F});
            return;
        }

        glm::vec3 r1 = wheelLTW.worldPosition() - carLTW.worldPosition();
        glm::vec3 tireWorldVelocity = carVelocity.vec + glm::cross(carAngVelocity.vec, r1);
        // gizmos.color({1.0F, 1.0F, 0.0F});
        // gizmos.drawLine("line", wheelLTW.worldPosition(), wheelLTW.worldPosition() + steeringDir);

        float steeringDirVelocity = glm::dot(steeringDir, tireWorldVelocity);

        float grip = wheel.gripFactor;

        float desiredVelocityChange = -steeringDirVelocity * grip;

        float xForce = carMass.mass * desiredVelocityChange;

        carForce.addForceOnPoint(steeringDir * xForce, wheelLTW.worldPosition() - carLTW.worldPosition(), carCOM.vec);

        // gizmos.color({1.0F, 0.0F, 0.0F});
        // gizmos.drawLine("line", wheelLTW.worldPosition(), wheelLTW.worldPosition() + steeringDir * xForce);
    }

    static void calculateAccelerationForces(const Wheel& wheel, const LocalToWorld& wheelLTW, const Car& car,
                                            const Mass& carMass, Force& carForce, const Velocity& carVelocity,
                                            const AngularVelocity& carAngVelocity, const CenterOfMass& carCOM,
                                            const LocalToWorld& carLTW)
    {
        if (car.handBrakeOn && wheel.axis != car.drivetrain)
        {
            glm::vec3 accelDir = carLTW.forward();
            float carSpeed = glm::dot(carLTW.forward(), carVelocity.vec);
            float resistance = carSpeed * carMass.mass / 2.0F;

            carForce.addForceOnPoint(-accelDir * resistance, wheelLTW.worldPosition() - carLTW.worldPosition(),
                                     carCOM.vec);
            return;
        }

        // accelerate
        if (wheel.axis != car.drivetrain)
        {
            return;
        }

        glm::vec3 accelDir = wheelLTW.forward();

        float carSpeed = glm::dot(carLTW.forward(), carVelocity.vec);

        // limit power to max speed so it doesn't accelerate more
        float carEnginePower = carSpeed >= car.topSpeed || carSpeed <= car.minimumSpeed ? 0.0F : car.enginePower;

        float torque = car.accelInput * carEnginePower;

        carForce.addForceOnPoint(accelDir * torque, wheelLTW.worldPosition() - carLTW.worldPosition(), carCOM.vec);

        // gizmos.color({1.0F, 0.0F, 0.0F});
        // gizmos.drawLine("line", wheelLTW.worldPosition(), wheelLTW.worldPosition() + accelDir * torque);

        // deccelerate
        if (car.accelInput == 0.0F)
        {
            glm::vec3 r1 = wheelLTW.worldPosition() - carLTW.worldPosition();
            glm::vec3 tireWorldVelocity = carVelocity.vec + glm::cross(carAngVelocity.vec, r1);

            float resistance = glm::dot(accelDir, tireWorldVelocity) * carMass.mass / car.resistanceMassRatio;

            carForce.addForceOnPoint(-accelDir * resistance, wheelLTW.worldPosition() - carLTW.worldPosition(),
                                     carCOM.vec);
        }
    }
} // namespace coffee

void coffee::carPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(gizmosPlugin);

    cubos.component<Car>();
    cubos.component<Wheel>();

    // system for turning wheel ()
    // query wheels and input, if wheels are front rotate according to input.
    cubos.system("read input and animate wheels")
        .call([](Input& input, DeltaTime& dt,
                 Query<Rotation&, ChildOf&, Position&, ChildOf&, const Wheel&, Rotation&, ChildOf&, Car&,
                       const LocalToWorld&, const Velocity&>
                     query) {
            for (auto [modelRotation, childOf1, modelPosition, childOf2, wheel, axleRotation, childOf3, car, carLTW,
                       carVelocity] : query)
            {
                float wheelAngularVelocity = wheel.currentVelocity / car.wheelRadius;
                modelRotation.quat *= glm::angleAxis(wheelAngularVelocity * dt.value(), glm::vec3(1.0F, 0.0F, 0.0F));

                auto desiredWheelHeight = -glm::max(wheel.currentSuspensionHeight, car.minSuspensionHeight);
                auto wheelHeightAlpha = 1.0F - glm::pow(0.5F, dt.value() / car.suspensionHalfTime);
                modelPosition.vec.y = glm::mix(modelPosition.vec.y, desiredWheelHeight, wheelHeightAlpha);

                if (wheel.axis != 0)
                {
                    axleRotation.quat = glm::quat({0.0F, 0.0F, 0.0F});
                    continue;
                }

                // save accel
                car.accelInput = -input.axis("move-y", car.playerOwner);

                // handbrake
                car.handBrakeOn = input.pressed("shoot", car.playerOwner);

                // Get the user's input.
                float steeringInput = -input.axis("move-x", car.playerOwner);
                car.steerInput = steeringInput;

                float carSpeed = glm::abs(glm::dot(carLTW.forward(), carVelocity.vec));

                float steeringAngle = glm::lerp(glm::pi<float>() / 2.5F, glm::pi<float>() / 6.0F,
                                                glm::clamp(carSpeed / 25.0F, 0.0F, 1.0F));
                if (car.handBrakeOn)
                {
                    steeringAngle = glm::pi<float>() / 2.5F;
                }

                float steeringRotation = steeringInput * steeringAngle;
                auto wheelAngleAlpha = 1.0F - glm::pow(0.5F, dt.value() / car.turnHalfTime);

                if (steeringRotation != 0)
                {
                    auto newRotation = steeringRotation;
                    axleRotation.quat =
                        glm::slerp(axleRotation.quat, glm::quat({0.0F, newRotation, 0.0F}), wheelAngleAlpha);
                }
                else
                {
                    axleRotation.quat = glm::slerp(axleRotation.quat, glm::quat({0.0F, 0.0F, 0.0F}), wheelAngleAlpha);
                }
            }
        });

    cubos.system("calculate wheel forces")
        .tagged(physicsApplyForcesTag)
        .call([](Gizmos& gizmos,
                 Query<Wheel&, const Position&, const LocalToWorld&, ChildOf&, const Car&, const Rotation&, Velocity&,
                       const AngularVelocity&, Force&, Torque&, const Mass&, const CenterOfMass&, const LocalToWorld&>
                     wheels,
                 Raycast raycast) {
            for (auto [wheel, wheelPosition, wheelLTW, childOf, car, carRotation, carVelocity, carAngVelocity, carForce,
                       carTorque, carMass, carCOM, carLTW] : wheels)
            {
                calculateGravity(wheelLTW, carMass, carForce, carCOM, carLTW);

                Raycast::Ray ray{.origin = wheelLTW.worldPosition(), .direction = -wheelLTW.up(), .mask = 0x00000002};
                auto hit = raycast.fire(ray);

                // suspension force
                if (hit)
                {
                    wheel.currentSuspensionHeight =
                        glm::min(glm::distance(hit->point, ray.origin), car.suspensionRestDist);
                    if (wheel.currentSuspensionHeight >= car.suspensionRestDist)
                    {
                        continue;
                    }

                    glm::vec3 r1 = wheelLTW.worldPosition() - carLTW.worldPosition();
                    glm::vec3 tireWorldVelocity = carVelocity.vec + glm::cross(carAngVelocity.vec, r1);
                    wheel.currentVelocity = glm::dot(wheelLTW.forward(), tireWorldVelocity);

                    // gizmos.color({0.0F, 0.0F, 1.0F});
                    // gizmos.drawLine("line", wheelLTW.worldPosition(), hit->point);

                    calculateSuspensionForces(wheel.currentSuspensionHeight, wheelLTW, car, carMass, carForce,
                                              carVelocity, carAngVelocity, carCOM, carLTW);

                    calculateSteeringForces(wheel, wheelLTW, car, carMass, carForce, carTorque, carVelocity,
                                            carAngVelocity, carCOM, carLTW);

                    calculateAccelerationForces(wheel, wheelLTW, car, carMass, carForce, carVelocity, carAngVelocity,
                                                carCOM, carLTW);
                }
            }
        });

    /*
    cubos.system("apply gravity")
        .tagged(physicsApplyForcesTag)
        .call([](Query<Velocity&, Force&, const Mass&, const Car&> query) {
            for (auto [velocity, force, mass, car] : query)
            {
                // Apply gravity force
                glm::vec3 gravitationForce = mass.mass * glm::vec3(0.0F, -15.0F, 0.0F);

                force.add(gravitationForce);
            }
        });*/
}