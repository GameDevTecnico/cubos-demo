#include "victory.hpp"

#include <cubos/core/ecs/component/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Victory)
{
    return cubos::core::ecs::ComponentTypeBuilder<Victory>("demo::Victory")
        .withField("nextScene", &Victory::nextScene)
        .build();
}
