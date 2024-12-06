#include "plugin.hpp"
#include "join.hpp"
#include "login.hpp"

void airships::server::eventsPlugin(cubos::engine::Cubos& cubos)
{
    cubos.event<LoginEvent>();
    cubos.event<JoinEvent>();
}
