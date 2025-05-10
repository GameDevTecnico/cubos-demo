#include "plugin.hpp"
#include "../toilet_paper/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(coffee::PlayerScores)
{
    return cubos::core::ecs::TypeBuilder<PlayerScores>("coffee::PlayerScores").build();
}

void coffee::scorePlugin(Cubos& cubos)
{
    cubos.depends(toiletPaperPlugin);

    cubos.resource<PlayerScores>();

    cubos.system("give score to player holding toilet paper")
        .call([](Query<ToiletPaper&> query, const DeltaTime& dt, PlayerScores& scores) {
            scores.scoreTimer -= dt.value();
            for (auto [toiletPaper] : query)
            {
                if (toiletPaper.player != -1 && scores.scoreTimer <= 0.0F)
                {
                    scores.scores[toiletPaper.player - 1] += 1;
                }
            }
            if (scores.scoreTimer <= 0.0F)
            {
                scores.scoreTimer = 0.1F;
            }
        });
}
