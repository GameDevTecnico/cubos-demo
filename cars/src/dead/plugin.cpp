#include "plugin.hpp"
#include "dead.hpp"

using namespace cubos::engine;

void demo::deadPlugin(Cubos& cubos)
{
    cubos.component<Dead>();
}
