#include "plugin.hpp"

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/physics/plugin.hpp>
#include <cubos/engine/physics/plugins/gravity.hpp>

#include <iostream>

using namespace cubos::engine;

static const Asset<Scene> CannonAsset = AnyAsset("a7bccdfb-0d74-4607-93e3-3fd4eb742d51");
static const Asset<Scene> CannonBall = AnyAsset("a7bccdfb-0d74-4607-93e3-3fd4eb742d51");

namespace airships::client
{

    CUBOS_REFLECT_IMPL(Cannon)
    {
        return cubos::core::ecs::TypeBuilder<Cannon>("Cannon")
            .withField("cannonLoaded", &Cannon::cannonLoaded)
            .build();
    }

    void cannonPlugin(Cubos& cubos)
    {
        cubos.depends(assetsPlugin);
        cubos.depends(inputPlugin);
        cubos.depends(transformPlugin);
        cubos.depends(gravityPlugin);

        cubos.component<Cannon>();

        cubos.startupSystem("cannon").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
            
            auto cannon1 = cmds.spawn(assets.read(CannonAsset)->blueprint);
            cannon1.add("cannon", Position{.vec = {-50.0F, 0.0F, 0.0F}});
            cannon1.add("cannon", Rotation{.quat = {1.0F, 0.0F, 0.0F, 0.0F}});
            cannon1.add("cannon", Cannon{.player = 1});

            auto cannon2 = cmds.spawn(assets.read(CannonAsset)->blueprint);
            cannon2.add("cannon", Position{.vec = {-50.0F, 0.0F, 0.0F}});
            cannon2.add("cannon", Rotation{.quat = {1.0F, 0.0F, 0.0F, 0.0F}});
            cannon2.add("cannon", Cannon{.player = 1});
        });

        cubos.system("cannon controls")
            .call([](Commands cmds, Assets& assets, Input& inputs, const DeltaTime& dt, Query<Cannon&, Rotation&> cannonRot) {

            for (auto [cannon, rotation] : cannonRot)
            {

                glm::quat yawQuat = glm::quat(1.0F, 0.0F, 0.0F, 0.0F); // Rotation around Y-axis
                glm::quat pitchQuat = glm::quat(1.0F, 0.0F, 0.0F, 0.0F); // Rotation around X-axis

                if (inputs.pressed("left", cannon.player))
                {
                    yawQuat = glm::angleAxis(0.5F * dt.value(), glm::vec3(0.0F, 1.0F, 0.0F));
                }
                if (inputs.pressed("right", cannon.player))
                {
                    yawQuat = glm::angleAxis(-0.5F * dt.value(), glm::vec3(0.0F, 1.0F, 0.0F));
                }
                if (inputs.pressed("down", cannon.player))
                {
                    pitchQuat = glm::angleAxis(0.5F * dt.value(), glm::vec3(1.0F, 0.0F, 0.0F));
                }
                if (inputs.pressed("up", cannon.player))
                {
                    pitchQuat = glm::angleAxis(-0.5F * dt.value(), glm::vec3(1.0F, 0.0F, 0.0F));
                }

                // Combine the yaw and pitch rotations
                rotation.quat = yawQuat * pitchQuat * rotation.quat;

                // Remove roll (Z-axis rotation) to keep the cannon parallel to the X-Z plane
                glm::vec3 euler = glm::eulerAngles(rotation.quat);
                rotation.quat = glm::quat(glm::vec3(euler.x, euler.y, 0.0F));

                if (inputs.justPressed("reload")) {
                    cannon.cannonLoaded = true;
                }
            }
        });

        cubos.system("fire cannon")
            .call([](Commands cmds, Assets& assets, Input& inputs, Query<Cannon&, const Rotation&, const Position&> cannonRotPos) {
                
                for(auto [cannon, rotation, position] : cannonRotPos)
                {
                    if (inputs.justPressed("fire", cannon.player)) {
                        if (!cannon.cannonLoaded) {
                            return;
                        }

                        glm::vec3 forward = rotation.quat * glm::vec3(0.0F, 0.0F, -1.0F);

                        auto ball = cmds.spawn(assets.read(CannonBall)->blueprint);
                        ball.add("cannon", Position{.vec = position.vec});
                        ball.add("cannon", PhysicsBundle{.mass = 10.0F, .impulse = forward * 1000.0F});

                        cannon.cannonLoaded = false;
                    }
                }
            });
    }
} // namespace airships::client