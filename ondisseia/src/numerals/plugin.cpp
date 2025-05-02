#include "plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/string.hpp>

#include <cubos/engine/prelude.hpp>
#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/scene/plugin.hpp>
#include <cubos/engine/assets/plugin.hpp>

#include <random>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Numeral)
{
    return cubos::core::ecs::TypeBuilder<Numeral>("demo::Numeral")
        .withField("model0", &Numeral::model0)
        .withField("modelI", &Numeral::modelI)
        .withField("modelV", &Numeral::modelV)
        .withField("modelX", &Numeral::modelX)
        .withField("number", &Numeral::number)
        .build();
}

void demo::numeralsPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(scenePlugin);
    cubos.depends(assetsPlugin);

    cubos.component<Numeral>();

    cubos.observer("create Numeral")
        .onAdd<Numeral>()
        .call([](Commands cmds, Assets& assets, Query<Entity, const Numeral&> query) {
            for (auto [ent, numeral] : query)
            {
                std::string digits = "";
                switch (numeral.number)
                {
                case 0:
                    digits = "O";
                    break;
                case 1:
                    digits = "I";
                    break;
                case 2:
                    digits = "II";
                    break;
                case 3:
                    digits = "III";
                    break;
                case 4:
                    digits = "IV";
                    break;
                case 5:
                    digits = "V";
                    break;
                case 6:
                    digits = "VI";
                    break;
                case 7:
                    digits = "VII";
                    break;
                case 8:
                    digits = "VIII";
                    break;
                case 9:
                    digits = "IX";
                    break;
                case 10:
                    digits = "X";
                    break;
                }

                float distance = 0.9F;
                float offset = (float)digits.size() * distance / 2.0F;

                for (int i = 0; i < digits.size(); ++i)
                {
                    Asset<Scene> asset;
                    switch (digits[i])
                    {
                    case 'O':
                        asset = numeral.model0;
                        break;
                    case 'I':
                        asset = numeral.modelI;
                        break;
                    case 'V':
                        asset = numeral.modelV;
                        break;
                    case 'X':
                        asset = numeral.modelX;
                        break;
                    default:
                        continue;
                    }

                    auto digitEnt =
                        cmds.spawn(*assets.read(asset)).add(Position{{(float)i * -distance + offset, 0, 0}}).entity();
                    cmds.relate(digitEnt, ent, ChildOf{});
                }

                cmds.remove<Numeral>(ent);
            }
        });
}
