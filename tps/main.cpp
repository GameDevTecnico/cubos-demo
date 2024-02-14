#include "health/plugin.hpp"

using namespace cubos::engine;

int main(int argc, char** argv)
{
    cubos.addPlugin(healthPlugin);
}
