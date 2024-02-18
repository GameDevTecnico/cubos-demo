#include "plugin.hpp"
#include "flag.hpp"
#include "../tank/tank.hpp"

#include <glm/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

void flagPlugin(Cubos& cubos)
{
    cubos.addComponent<Flag>();

    cubos.system("find players close to flag")
        .call([](Query<Flag&, const Position&> query,
                 Query<const TankBody&, const Position&, const Rotation&, const Tank&, const TankTower&,
                       const Position&, const Rotation&>
                     playerQuery) {
            for (auto [flag, pos] : query)
            {
                flag.isClosePlayer1 = false;
                flag.isClosePlayer2 = false;
                for (auto [body, bodyPosition, bodyRotation, tank, tower, towerPosition, towerRotation] : playerQuery)
                {
                    float dist = glm::distance(pos.vec, bodyPosition.vec);
                    if (dist <= flag.range)
                    {
                        switch (tank.player)
                        {
                        case 1:
                            flag.isClosePlayer1 = true;
                            break;

                        case 2:
                            flag.isClosePlayer2 = true;
                            break;

                        default:
                            break;
                        }
                    }
                }
            }
        });

    cubos.system("update progress to win").call([](const DeltaTime& dt, Query<Flag&> query) {
        for (auto [flag] : query)
        {
            if (flag.isClosePlayer1 && flag.isClosePlayer2)
            {
                continue;
            }
            else if (flag.isClosePlayer1)
            {
                flag.progressPlayer1 += dt.value;
                flag.progressPlayer2 -= 2.0F * dt.value;
            }
            else if (flag.isClosePlayer2)
            {
                flag.progressPlayer2 += dt.value;
                flag.progressPlayer1 -= 2.0F * dt.value;
            }
            else
            {
                flag.progressPlayer1 -= dt.value;
                flag.progressPlayer2 -= dt.value;
            }

            if (flag.progressPlayer1 < 0)
                flag.progressPlayer1 = 0;
            if (flag.progressPlayer2 < 0)
                flag.progressPlayer2 = 0;
        }
    });
}
