#include "plugin.hpp"
#include "../bullet/bullet.hpp"
#include <cubos/engine/input/input.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/transform/plugin.hpp>

#include <cubos/engine/assets/asset.hpp>
#include <cubos/engine/assets/assets.hpp>
#include <cubos/engine/physics/plugin.hpp>

using namespace cubos::engine;

static const Asset<Scene> BulletSceneAsset = AnyAsset("bf49ba61-5103-41bc-92e0-8a331d7842f6");

void gunPlugin(Cubos& cubos)
{
    cubos.addComponent<Gun>();

    cubos.system("shoot projectile")
        .call([](Commands cmds, const DeltaTime& dt, const Input& input, const Assets& assets,
                 Query<Gun&, LocalToWorld&, ChildOf&, Rotation&> query) {
            // increase bullet time
            for (auto [gun, localToWorld, childOf, rotation] : query)
            {
                gun.timeSinceLastShot += dt.value;

                // shoot if we can and want to
                if (input.pressed("shoot", gun.player) && gun.timeSinceLastShot >= gun.minTimeBetweenShots)
                {
                    auto builder = cmds.spawn(assets.read(BulletSceneAsset)->blueprint);
                    builder.add("bullet", Position{.vec = localToWorld.mat[3]});
                    builder.add("bullet", Rotation{.quat = rotation.quat});
                    auto impulse = Impulse{};
                    impulse.add(glm::vec3(glm::mat4(rotation.quat) * glm::vec4(gun.bulletImpulse, 1.0F)));
                    builder.add("bullet", impulse);
                    gun.timeSinceLastShot = 0.0F;
                }
            }
        });
}