#include "plugin.hpp"
#include "player.hpp"

using namespace cubos::engine;

void playerPlugin(Cubos& cubos)
{
    cubos.component<Player>();
}
