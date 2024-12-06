#pragma once

#include <cubos/core/ecs/entity/entity.hpp>

namespace airships::server
{
    struct JoinEvent
    {
        cubos::core::ecs::Entity entity;
    };
} // namespace airships::server
