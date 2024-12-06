#pragma once

#include <string>
#include <cubos/core/ecs/entity/entity.hpp>

namespace airships::server
{
    struct LoginEvent
    {
        cubos::core::ecs::Entity entity;
        std::string name;
    };
} // namespace airships::server
