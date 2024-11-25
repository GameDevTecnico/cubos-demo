#include "plugin.hpp"
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/fixed_step/fixed_delta_time.hpp>
#include <cubos/engine/settings/plugin.hpp>

static constexpr uint16_t DefaultPort = 26000;
static constexpr float DefaultTickrate = 1.0F / 64.0F;

CUBOS_DEFINE_TAG(airships::server::serverTickStartTag);
CUBOS_DEFINE_TAG(airships::server::serverTickTag);
CUBOS_DEFINE_TAG(airships::server::serverTickEndTag);

namespace airships::server
{
    void gameServerPlugin(cubos::engine::Cubos& cubos)
    {
        cubos.depends(cubos::engine::fixedStepPlugin);
        cubos.depends(cubos::engine::settingsPlugin);

        cubos.tag(serverTickTag).after(serverTickStartTag);
        cubos.tag(serverTickEndTag).after(serverTickTag);

        /// @todo initialize socket
        cubos.startupSystem("setup server")
            .call([](cubos::engine::Settings& settings, cubos::engine::FixedDeltaTime& dt,
                     cubos::engine::ShouldQuit& quit) {
                auto port = static_cast<uint16_t>(settings.getInteger("airships.server.port", DefaultPort));
                auto tickrate = static_cast<float>(settings.getDouble("airships.server.tickrate", DefaultTickrate));
                dt.value = tickrate;
                quit.value = false;
            });

        cubos.system("start server tick").tagged(cubos::engine::fixedStepTag).tagged(serverTickStartTag).call([]() {
            /// @todo use packet receive pipe to read all incoming packets.
        });

        cubos.system("end server tick")
            .tagged(cubos::engine::fixedStepTag)
            .tagged(serverTickEndTag)
            .call([](cubos::engine::FixedDeltaTime& dt) {
                /// @todo use packet send pipe to send all packets to clients
            });
    }
} // namespace airships::server
