#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"

#include <random>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Turret)
{
    return cubos::core::ecs::TypeBuilder<Turret>("demo::Turret")
        .withField("bullet", &Turret::bullet)
        .withField("root", &Turret::root)
        .withField("cooldown", &Turret::cooldown)
        .withField("timeSinceLastShot", &Turret::timeSinceLastShot)
        .withField("rotationTime", &Turret::rotationTime)
        .withField("ammo", &Turret::ammo)
        .withField("maxAmmoForReload", &Turret::maxAmmoForReload)
        .withField("ammoPerReload", &Turret::ammoPerReload)
        .build();
}

CUBOS_REFLECT_IMPL(demo::Ammo)
{
    return cubos::core::ecs::TypeBuilder<Ammo>("demo::Ammo").build();
}

void demo::turretPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(interactionPlugin);

    cubos.component<Turret>();
    cubos.component<Ammo>();

    cubos.observer("remove Turret gun when destroyed")
        .entity()
        .related<ChildOf>()
        .onRemove<Turret>()
        .call([](Commands cmds, Query<Entity> children) {
            for (auto [child] : children)
            {
                cmds.destroy(child);
            }
        });

    cubos.observer("reload Turret ammo when interacted with Ammo")
        .onAdd<Interaction>()
        .call([](Commands cmds, Query<const Interaction&, Turret&> turrets,
                 Query<Entity, const Ammo&, const ChildOf&> ammo) {
            for (auto [interaction, turret] : turrets)
            {
                for (auto [entity, ammo, childOf] : ammo.pin(1, interaction.entity))
                {
                    if (turret.ammo <= turret.maxAmmoForReload)
                    {
                        cmds.destroy(entity);
                        turret.ammo += turret.ammoPerReload;
                    }
                }
            }
        });
}
