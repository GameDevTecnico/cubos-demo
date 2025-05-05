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

CUBOS_REFLECT_IMPL(demo::Car)
{
    return cubos::core::ecs::TypeBuilder<Car>("demo::Car")
        .withField("playerOwner", &Car::playerOwner)
        .withField("drivetrain", &Car::drivetrain)
        .withField("topSpeed", &Car::topSpeed)
        .build();
}

CUBOS_REFLECT_IMPL(demo::Wheel)
{
    return cubos::core::ecs::TypeBuilder<Wheel>("demo::Wheel")
        .withField("axis", &Wheel::axis)
        .withField("gripFactor", &Wheel::gripFactor)
        .withField("mass", &Wheel::mass)
        .withField("brakeForce", &Wheel::brakeForce)
        .build();
}

namespace demo
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

    static void calculateSteeringForces(const Wheel& wheel, const LocalToWorld& wheelLTW, const Mass& carMass,
                                        Force& carForce, const Velocity& carVelocity,
                                        const AngularVelocity& carAngVelocity, const CenterOfMass& carCOM,
                                        const LocalToWorld& carLTW)
    {
        glm::vec3 steeringDir =
            glm::cross(wheelLTW.forward(), wheelLTW.up()); // we should be able to do wheelLTW.right()

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
} // namespace demo

void demo::carPlugin(Cubos& cubos)
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
    cubos.system("read input and turn wheels")
        .call([](Input& input, DeltaTime& dt, Query<const Wheel&, Rotation&, ChildOf&, Car&> query) {
            for (auto [wheel, rotation, childOf, car] : query)
            {
                if (wheel.axis != 0)
                {
                    rotation.quat = glm::quat({0.0F, 0.0F, 0.0F});
                    continue;
                }

                // save accel
                car.accelInput = -input.axis("move-y", car.playerOwner);

                // Get the user's input.
                float steeringInput = -input.axis("move-x", car.playerOwner);

                float steeringAngle = glm::pi<float>() / 6.0F;

                float steeringRotation = steeringInput * steeringAngle;

                if (steeringRotation != 0)
                {
                    auto newRotation = steeringRotation;

                    rotation.quat = glm::slerp(rotation.quat, glm::quat({0.0F, newRotation, 0.0F}), 0.3F);
                }
                else
                {
                    rotation.quat = glm::slerp(rotation.quat, glm::quat({0.0F, 0.0F, 0.0F}), 0.2F);
                }
            }
        });

    cubos.system("calculate wheel forces")
        .tagged(physicsApplyForcesTag)
        .call([](Gizmos& gizmos,
                 Query<const Wheel&, const Position&, const LocalToWorld&, ChildOf&, const Car&, const Rotation&,
                       Velocity&, const AngularVelocity&, Force&, Torque&, const Mass&, const CenterOfMass&,
                       const LocalToWorld&>
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
                    const float distance = glm::distance(hit->point, ray.origin);
                    if (distance > car.suspensionRestDist)
                    {
                        continue;
                    }

                    gizmos.color({0.0F, 0.0F, 1.0F});
                    gizmos.drawLine("line", wheelLTW.worldPosition(), hit->point);

                    calculateSuspensionForces(distance, wheelLTW, car, carMass, carForce, carVelocity, carAngVelocity,
                                              carCOM, carLTW);

                    calculateSteeringForces(wheel, wheelLTW, carMass, carForce, carVelocity, carAngVelocity, carCOM,
                                            carLTW);

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