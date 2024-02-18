#include "flag.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(Flag)
{
    return cubos::core::ecs::TypeBuilder<Flag>("Flag")
        .withField("range", &Flag::range)
        .withField("progressPlayer1", &Flag::progressPlayer1)
        .withField("progressPlayer2", &Flag::progressPlayer2)
        .withField("isClosePlayer1", &Flag::isClosePlayer1)
        .withField("isClosePlayer2", &Flag::isClosePlayer2)
        .build();
}
