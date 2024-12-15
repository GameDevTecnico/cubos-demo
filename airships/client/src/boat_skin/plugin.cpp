#include "plugin.hpp"
#include "../interpolation/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(airships::client::BoatSkin)
{
    return cubos::core::ecs::TypeBuilder<BoatSkin>("airships::client::BoatSkin")
        .withField("hull", &BoatSkin::hull)
        .withField("balloon", &BoatSkin::balloon)
        .withField("rudder", &BoatSkin::rudder)
        .withField("anchor", &BoatSkin::anchor)
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::BoatSkinHull)
{
    return cubos::core::ecs::TypeBuilder<BoatSkinHull>("airships::client::BoatSkinHull").build();
}

CUBOS_REFLECT_IMPL(airships::client::BoatSkinBalloon)
{
    return cubos::core::ecs::TypeBuilder<BoatSkinBalloon>("airships::client::BoatSkinBalloon").build();
}

CUBOS_REFLECT_IMPL(airships::client::BoatSkinRudder)
{
    return cubos::core::ecs::TypeBuilder<BoatSkinRudder>("airships::client::BoatSkinRudder").build();
}

CUBOS_REFLECT_IMPL(airships::client::BoatSkinAnchor)
{
    return cubos::core::ecs::TypeBuilder<BoatSkinAnchor>("airships::client::BoatSkinAnchor").build();
}

void airships::client::boatSkinPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(interpolationPlugin);

    cubos.component<BoatSkin>();
    cubos.component<BoatSkinHull>();
    cubos.component<BoatSkinBalloon>();
    cubos.component<BoatSkinRudder>();
    cubos.component<BoatSkinAnchor>();

    cubos.system("set boat skin for hull")
        .with<BoatSkinHull>()
        .call([](Commands cmds, Query<Entity, const ChildOf&, const InterpolationOf&, const BoatSkin&> query) {
            for (auto [entity, childOf, interpolationOf, skin] : query)
            {
                cmds.add(entity, skin.hull);
                cmds.remove<BoatSkinHull>(entity);
            }
        });

    cubos.system("set boat skin for balloon")
        .with<BoatSkinBalloon>()
        .call([](Commands cmds, Query<Entity, const ChildOf&, const InterpolationOf&, const BoatSkin&> query) {
            for (auto [entity, childOf, interpolationOf, skin] : query)
            {
                cmds.add(entity, skin.balloon);
                cmds.remove<BoatSkinBalloon>(entity);
            }
        });

    cubos.system("set boat skin for rudder")
        .with<BoatSkinRudder>()
        .call([](Commands cmds, Query<Entity, const ChildOf&, const InterpolationOf&, const BoatSkin&> query) {
            for (auto [entity, childOf, interpolationOf, skin] : query)
            {
                cmds.add(entity, skin.rudder);
                cmds.remove<BoatSkinRudder>(entity);
            }
        });

    cubos.system("set boat skin for anchor")
        .with<BoatSkinAnchor>()
        .call([](Commands cmds, Query<Entity, const ChildOf&, const InterpolationOf&, const BoatSkin&> query) {
            for (auto [entity, childOf, interpolationOf, skin] : query)
            {
                cmds.add(entity, skin.anchor);
                cmds.remove<BoatSkinAnchor>(entity);
            }
        });
}
