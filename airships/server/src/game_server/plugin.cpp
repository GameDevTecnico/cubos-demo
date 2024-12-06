#include "plugin.hpp"
#include <cubos/engine/fixed_step/plugin.hpp>
#include <cubos/engine/fixed_step/fixed_delta_time.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/engine/settings/plugin.hpp>
#include <cubos/core/net/tcp_listener.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include "game_server.hpp"

static constexpr uint16_t DefaultPort = 26001;
static constexpr float DefaultTickrate = 1.0F / 1.0F;

CUBOS_DEFINE_TAG(airships::server::serverTickStartTag);
CUBOS_DEFINE_TAG(airships::server::serverTickTag);
CUBOS_DEFINE_TAG(airships::server::serverTickEndTag);

namespace airships::server
{
    void gameServerPlugin(cubos::engine::Cubos& cubos)
    {
        cubos.depends(cubos::engine::fixedStepPlugin);
        cubos.depends(cubos::engine::settingsPlugin);

        cubos.tag(serverTickStartTag).tagged(cubos::engine::fixedStepTag);
        cubos.tag(serverTickTag).tagged(cubos::engine::fixedStepTag).after(serverTickStartTag);
        cubos.tag(serverTickEndTag).tagged(cubos::engine::fixedStepTag).after(serverTickTag);

        cubos.uninitResource<GameServer>();

        cubos.startupSystem("setup server")
            .call([](cubos::engine::Commands cmds, cubos::engine::Settings& settings, cubos::engine::FixedDeltaTime& dt,
                     cubos::engine::ShouldQuit& quit) {
                auto port = static_cast<uint16_t>(settings.getInteger("airships.server.port", DefaultPort));
                auto tickrate = static_cast<float>(settings.getDouble("airships.server.tickrate", DefaultTickrate));
                dt.value = tickrate;
                quit.value = false;
                cmds.emplaceResource<GameServer>(port);
            });

        cubos.system("start server tick").tagged(serverTickStartTag).call([](GameServer& server) {
            server.startTick();
            CUBOS_INFO("STARTED SERVER TICK");
        });

        cubos.system("end server tick")
            .tagged(serverTickEndTag)
            .call([](GameServer& server, cubos::engine::FixedDeltaTime& dt) {
                auto end = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::system_clock::now().time_since_epoch());
                auto elapsed = end - server.tickStart();
                if (elapsed.count() > dt.value)
                {
                    CUBOS_WARN("Tick took longer than expected: {} ms", elapsed.count());
                }
                CUBOS_INFO("ENDED SERVER TICK");
            });
    }
} // namespace airships::server
