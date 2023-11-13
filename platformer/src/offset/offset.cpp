#include "offset.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/glm.hpp>

CUBOS_REFLECT_IMPL(demo::Offset)
{
    return cubos::core::ecs::ComponentTypeBuilder<Offset>("demo::Offset")
        .withField("vec", &Offset::vec)
        .withField("parent", &Offset::parent)
        .build();
}
