#include "plugin.hpp"
#include "../follow/plugin.hpp"

#include <cubos/core/reflection/external/string.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/input/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>

using namespace cubos::core::io;
using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::FollowController)
{
    return cubos::core::ecs::TypeBuilder<FollowController>("airships::client::FollowController")
        .withField("player", &FollowController::player)
        .withField("zoomAxis", &FollowController::zoomAxis)
        .withField("phiAxis", &FollowController::phiAxis)
        .withField("thetaAxis", &FollowController::thetaAxis)
        .withField("minZoom", &FollowController::minZoom)
        .withField("maxZoom", &FollowController::maxZoom)
        .withField("zoomSpeed", &FollowController::zoomSpeed)
        .withField("rotationSpeed", &FollowController::rotationSpeed)
        .withField("useMouse", &FollowController::useMouse)
        .withField("mouseSensitivity", &FollowController::mouseSensitivity)
        .withField("scrollSensitivity", &FollowController::scrollSensitivity)
        .build();
}

void airships::client::followControllerPlugin(Cubos& cubos)
{
    cubos.depends(inputPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(followPlugin);

    cubos.component<FollowController>();

    cubos.system("update FollowController entities")
        .call([](const DeltaTime& dt, Input& input, Window& window, EventReader<WindowEvent> events,
                 Query<const FollowController&, Follow&> query) {
            int mouseScroll = 0;
            for (auto event : events)
            {
                if (auto scrollEvent = std::get_if<MouseScrollEvent>(&event))
                {
                    mouseScroll += scrollEvent->offset.y;
                }
            }

            for (auto [controller, follow] : query)
            {
                if (controller.player == -1)
                {
                    continue;
                }

                float zoomInput = input.axis(controller.zoomAxis.c_str(), controller.player);
                float phiInput = input.axis(controller.phiAxis.c_str(), controller.player);
                float thetaInput = input.axis(controller.thetaAxis.c_str(), controller.player);

                // Lock the mouse if it isn't already
                if (controller.useMouse && window->mouseState() != MouseState::Locked)
                {
                    window->mouseState(MouseState::Locked);
                }
                else if (!controller.useMouse && window->mouseState() == MouseState::Locked)
                {
                    window->mouseState(MouseState::Default);
                }

                if (controller.useMouse)
                {
                    zoomInput -= static_cast<float>(mouseScroll) * controller.scrollSensitivity * dt.value();
                    phiInput += input.mouseDelta().y * controller.mouseSensitivity * dt.value();
                    thetaInput -= input.mouseDelta().x * controller.mouseSensitivity * dt.value();
                }

                // Update the distance from the target
                follow.distance = glm::clamp(follow.distance, controller.minZoom, controller.maxZoom);
                auto zoomRange = controller.maxZoom - controller.minZoom;
                auto zoomFactor = glm::sqrt((follow.distance - controller.minZoom) / zoomRange);
                zoomFactor += zoomInput * controller.zoomSpeed * dt.value();
                zoomFactor = glm::clamp(zoomFactor, 0.0F, 1.0F);
                follow.distance = controller.minZoom + zoomFactor * zoomFactor * zoomRange;

                // Update the phi and theta angles
                follow.phi = glm::clamp(follow.phi + phiInput * controller.rotationSpeed * dt.value(), -89.0F, 89.0F);
                follow.theta = glm::mod(follow.theta - thetaInput * controller.rotationSpeed * dt.value(), 360.0F);
            }
        });
}