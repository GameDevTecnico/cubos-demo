#include "player.hpp"

#include <cubos/core/ecs/component/reflection.hpp>

#include <cubos/core/reflection/external/primitives.hpp>

CUBOS_REFLECT_IMPL(demo::Player)
{
    return cubos::core::ecs::ComponentTypeBuilder<Player>("demo::Player")
        .withField("id", &Player::id)
        .withField("isOnGround", &Player::isOnGround)
        .withField("animationTime", &Player::animationTime)
        .withField("torso", &Player::torso)
        .withField("leftHand", &Player::leftHand)
        .withField("rightHand", &Player::rightHand)
        .withField("leftFoot", &Player::leftFoot)
        .withField("rightFoot", &Player::rightFoot)
        .build();
}
