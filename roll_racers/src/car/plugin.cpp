#include "plugin.hpp"
#include "../interpolation/plugin.hpp"

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
#include <cubos/engine/render/lights/plugin.hpp>
#include <cubos/engine/render/lights/point.hpp>
#include <cubos/engine/render/lights/spot.hpp>
#include <cubos/engine/audio/plugin.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(roll_racers::PlayerOwner)
{
    return cubos::core::ecs::TypeBuilder<PlayerOwner>("roll_racers::PlayerOwner")
        .withField("player", &PlayerOwner::player)
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::PlayerCameraOwner)
{
    return cubos::core::ecs::TypeBuilder<PlayerCameraOwner>("roll_racers::PlayerCameraOwner")
        .withField("player", &PlayerCameraOwner::player)
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::Car)
{
    return cubos::core::ecs::TypeBuilder<Car>("roll_racers::Car")
        .withField("drivetrain", &Car::drivetrain)
        .withField("accelInput", &Car::accelInput)
        .withField("lowBackLightIntensity", &Car::lowBackLightIntensity)
        .withField("highBackLightIntensity", &Car::highBackLightIntensity)
        .withField("headLightIntensity", &Car::headLightIntensity)
        .withField("lightIntensityHalfTime", &Car::lightIntensityHalfTime)
        .withField("dragConstant", &Car::dragConstant)
        .withField("rollingResistanceConstant", &Car::rollingResistanceConstant)
        .withField("suspensionRestDist", &Car::suspensionRestDist)
        .withField("wheelRadius", &Car::wheelRadius)
        .withField("springStrength", &Car::springStrength)
        .withField("springDamper", &Car::springDamper)
        .withField("minSuspensionHeight", &Car::minSuspensionHeight)
        .withField("suspensionHalfTime", &Car::suspensionHalfTime)
        .withField("fastWheelSteeringAngle", &Car::fastWheelSteeringAngle)
        .withField("idleWheelSteeringAngle", &Car::idleWheelSteeringAngle)
        .withField("fastWheelSteeringVelocity", &Car::fastWheelSteeringVelocity)
        .withField("currentWheelSteeringAngle", &Car::currentWheelSteeringAngle)
        .withField("wheelSteeringHalfTime", &Car::wheelSteeringHalfTime)
        .withField("tractionConstant", &Car::tractionConstant)
        .withField("tractionPeakSlip", &Car::tractionPeakSlip)
        .withField("corneringConstant", &Car::corneringConstant)
        .withField("corneringPeakSlipAngle", &Car::corneringPeakSlipAngle)
        .withField("driveTorque", &Car::driveTorque)
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::Wheel)
{
    return cubos::core::ecs::TypeBuilder<Wheel>("roll_racers::Wheel")
        .withField("axis", &Wheel::axis)
        .withField("gripFactor", &Wheel::gripFactor)
        .withField("mass", &Wheel::mass)
        .withField("brakeForce", &Wheel::brakeForce)
        .build();
}

CUBOS_REFLECT_IMPL(roll_racers::Horn)
{
    return cubos::core::ecs::TypeBuilder<Horn>("roll_racers::Horn").build();
}

namespace roll_racers
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

    static void calculateTractionForces(Wheel& wheel, const LocalToWorld& wheelLTW, const Car& car, const Mass& carMass,
                                        Force& carForce, Torque& carTorque, const Velocity& carVelocity,
                                        const AngularVelocity& carAngVelocity, const CenterOfMass& carCOM,
                                        const LocalToWorld& carLTW, const FixedDeltaTime& dt,
                                        const PlayerOwner& carOwner)
    {
        auto right = glm::cross(wheelLTW.forward(), wheelLTW.up()); // we should be able to do wheelLTW.right()
        float lateralVelocity = glm::dot(right, carVelocity.vec);
        float longitudinalVelocity = glm::dot(wheelLTW.forward(), carVelocity.vec);

        float slipRatio =
            (wheel.angularVelocity * car.wheelRadius - longitudinalVelocity) / glm::abs(longitudinalVelocity);

        float wheelOffset = glm::dot(carLTW.worldPosition() - wheelLTW.worldPosition(), carLTW.forward());
        float slipAngle =
            glm::abs(longitudinalVelocity) > 0.0F
                ? glm::atan((lateralVelocity + carAngVelocity.vec.y * wheelOffset) / glm::abs(longitudinalVelocity))
                : 0.0F;

        float tractionForce = car.tractionConstant * glm::clamp(slipRatio, -car.tractionPeakSlip, car.tractionPeakSlip);
        float tractionTorque = tractionForce * car.wheelRadius;

        float wheelTorque = -tractionTorque + (wheel.axis == car.drivetrain ? car.driveTorque * car.accelInput : 0.0F);
        float wheelInertia = wheel.mass * car.wheelRadius * car.wheelRadius * 0.5F;
        float wheelAngularAcceleration = wheelTorque / wheelInertia;
        wheel.angularVelocity += wheelAngularAcceleration * dt.value;

        if ((car.handBrakeOn && wheel.axis == 1))
        {
            wheel.angularVelocity = 0.0F;
        }

        if (carOwner.canMove)
        {
            carForce.addForceOnPoint(tractionForce * wheelLTW.forward(),
                                     wheelLTW.worldPosition() - carLTW.worldPosition(), carCOM.vec);

            float lateralForce =
                -car.corneringConstant * glm::clamp(slipAngle, -glm::radians(car.corneringPeakSlipAngle),
                                                    glm::radians(car.corneringPeakSlipAngle));
            carForce.addForceOnPoint(lateralForce * right, wheelLTW.worldPosition() - carLTW.worldPosition(),
                                     carCOM.vec);
        }
        else
        {
            wheel.angularVelocity = 0.0F;
        }
    }
} // namespace roll_racers

void roll_racers::carPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(inputPlugin);
    cubos.depends(collisionsPlugin);
    cubos.depends(fixedStepPlugin);
    cubos.depends(physicsPlugin);
    cubos.depends(gizmosPlugin);
    cubos.depends(interpolationPlugin);
    cubos.depends(lightsPlugin);
    cubos.depends(audioPlugin);

    cubos.component<PlayerOwner>();
    cubos.component<PlayerCameraOwner>();
    cubos.component<Car>();
    cubos.component<Wheel>();
    cubos.component<Horn>();

    // system for turning wheel ()
    // query wheels and input, if wheels are front rotate according to input.
    cubos.system("read input and animate wheels")
        .call([](Commands cmds, Input& input, DeltaTime& dt, Query<Car&> carQuery,
                 Query<Rotation&, ChildOf&, Position&, ChildOf&, const Wheel&, Rotation&, ChildOf&, Entity, Car&,
                       AudioSource&, const PlayerOwner&, const LocalToWorld&, const Velocity&>
                     query,
                 Query<PointLight&, const ChildOf&, const InterpolationOf&, const Car&> backLights,
                 Query<SpotLight&, const ChildOf&, const InterpolationOf&, const Car&> headLights,
                 Query<Entity, const AudioSource&, const Horn&, const ChildOf&> hornQuery) {
            for (auto [modelRotation, childOf1, modelPosition, childOf2, wheel, axleRotation, childOf3, carEnt, car,
                       audio, carOwner, carLTW, carVelocity] : query)
            {
                modelRotation.quat *= glm::angleAxis(wheel.angularVelocity * dt.value(), glm::vec3(1.0F, 0.0F, 0.0F));

                auto desiredWheelHeight = -glm::max(wheel.currentSuspensionHeight, car.minSuspensionHeight);
                auto wheelHeightAlpha = 1.0F - glm::pow(0.5F, dt.value() / car.suspensionHalfTime);
                modelPosition.vec.y = glm::mix(modelPosition.vec.y, desiredWheelHeight, wheelHeightAlpha);

                if (wheel.axis != 0)
                {
                    axleRotation.quat = glm::quat({0.0F, 0.0F, 0.0F});
                    continue;
                }

                // save accel
                car.accelInput = input.axis("throttle", carOwner.player);
                car.accelInput -= input.axis("reverse", carOwner.player);

                // handbrake
                car.handBrakeOn = input.pressed("handbrake", carOwner.player);

                // Get the user's input.
                car.steerInput = -input.axis("steer", carOwner.player);

                // Set the backlight intensity depending on the input.
                float targetBacklightIntensity = car.lowBackLightIntensity;
                if (carOwner.canMove)
                {
                    if (car.accelInput < 0.0F || car.handBrakeOn)
                    {
                        targetBacklightIntensity = car.highBackLightIntensity;
                    }
                }
                else
                {
                    targetBacklightIntensity = car.lowBackLightIntensity;
                }
                for (auto [light, childOf, interpolation, car] : backLights.pin(2, carEnt))
                {
                    light.intensity = glm::mix(light.intensity, targetBacklightIntensity,
                                               1.0F - glm::pow(0.5F, dt.value() / car.lightIntensityHalfTime));
                }

                // Set headlight intensity.
                float targetHeadLightIntensity = 0.0F;
                if (carOwner.canMove)
                {
                    targetHeadLightIntensity = car.headLightIntensity;
                }
                for (auto [light, childOf, interpolation, car] : headLights.pin(2, carEnt))
                {
                    light.intensity = glm::mix(light.intensity, targetHeadLightIntensity,
                                               1.0F - glm::pow(0.5F, dt.value() / car.lightIntensityHalfTime));
                }

                // Play cool audio.
                float carSpeed = glm::length(glm::vec3{carVelocity.vec.x, 0.0F, carVelocity.vec.z});
                float engineWork = carSpeed + glm::abs(car.accelInput) * 20.0F;
                int gear = static_cast<int>(glm::floor(engineWork / 20.0F));
                float rpm = (glm::mod(engineWork, 20.0F) / 20.0F);
                float targetPitch = 0.5F + gear * 0.5F + rpm * 0.5F;
                float targetGain = 1.5F + gear * 1.5F + rpm * 1.0F;
                targetGain /= carQuery.count();
                float audioHalfTime = 0.1F;
                float audioAlpha = 1.0F - glm::pow(0.5F, dt.value() / audioHalfTime);
                audio.pitch = glm::mix(audio.pitch, targetPitch, audioAlpha);
                audio.gain = glm::mix(audio.gain, targetGain, audioAlpha);
                cmds.add(carEnt, AudioPlay{});

                // Play horn audio.
                if (input.justPressed("horn", carOwner.player))
                {
                    for (auto [hornEnt, _1, _2, _3] : hornQuery.pin(1, carEnt))
                    {
                        cmds.add(hornEnt, AudioPlay{});
                    }
                }

                float fastWheelSteeringRatio = glm::clamp(carSpeed / car.fastWheelSteeringAngle, 0.0F, 1.0F);
                float maxWheelSteeringAngle =
                    glm::mix(car.idleWheelSteeringAngle, car.fastWheelSteeringAngle, fastWheelSteeringRatio);
                float desiredWheelAngle = car.steerInput * maxWheelSteeringAngle;

                auto wheelAngleAlpha = 1.0F - glm::pow(0.5F, dt.value() / car.wheelSteeringHalfTime);
                car.currentWheelSteeringAngle =
                    glm::mix(car.currentWheelSteeringAngle, desiredWheelAngle, wheelAngleAlpha);
                axleRotation.quat = glm::quat({0.0F, glm::radians(car.currentWheelSteeringAngle), 0.0F});
            }
        });

    cubos.system("calculate wheel forces")
        .tagged(physicsApplyForcesTag)
        .call([](Gizmos& gizmos,
                 Query<Wheel&, const Position&, const LocalToWorld&, ChildOf&, const Car&, const PlayerOwner&,
                       Rotation&, Velocity&, AngularVelocity&, Force&, Torque&, const Mass&, const CenterOfMass&,
                       const LocalToWorld&>
                     wheels,
                 Raycast raycast, const FixedDeltaTime& dt) {
            for (auto [wheel, wheelPosition, wheelLTW, childOf, car, carOwner, carRotation, carVelocity, carAngVelocity,
                       carForce, carTorque, carMass, carCOM, carLTW] : wheels)
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

                    calculateTractionForces(wheel, wheelLTW, car, carMass, carForce, carTorque, carVelocity,
                                            carAngVelocity, carCOM, carLTW, dt, carOwner);

                    if (!carOwner.canMove)
                    {
                        carVelocity.vec.x = 0.0F;
                        carVelocity.vec.z = 0.0F;
                        carAngVelocity.vec.y = 0.0F;
                    }
                }
            }
        });

    cubos.system("apply car drag")
        .tagged(physicsApplyForcesTag)
        .call([](Query<const LocalToWorld&, const Car&, AngularVelocity&, Torque&, Velocity&, Force&> query) {
            for (auto [carLTW, car, carAngularVelocity, carTorque, carVelocity, carForce] : query)
            {
                float carSpeed = glm::dot(carLTW.forward(), carVelocity.vec);
                carForce.add(-carSpeed * car.dragConstant * carVelocity.vec);
                carForce.add(-car.rollingResistanceConstant * carVelocity.vec);
            }
        });
}