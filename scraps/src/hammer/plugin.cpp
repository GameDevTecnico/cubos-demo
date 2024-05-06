#include "plugin.hpp"
#include "../interaction/plugin.hpp"
#include "../object/plugin.hpp"
#include "../holdable/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>

#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;

CUBOS_REFLECT_IMPL(demo::Hammer)
{
    return cubos::core::ecs::TypeBuilder<Hammer>("demo::Hammer").build();
}

CUBOS_REFLECT_IMPL(demo::Recipe)
{
    return cubos::core::ecs::TypeBuilder<Recipe>("demo::Recipe")
        .withField("result", &Recipe::result)
        .withField("root", &Recipe::root)
        .build();
}

CUBOS_REFLECT_IMPL(demo::Ingredient)
{
    return cubos::core::ecs::TypeBuilder<Ingredient>("demo::Ingredient")
        .withField("scene", &Ingredient::scene)
        .withField("root", &Ingredient::root)
        .build();
}

CUBOS_REFLECT_IMPL(demo::IsIngredientAsset)
{
    return cubos::core::ecs::TypeBuilder<IsIngredientAsset>("demo::IsIngredientAsset")
        .withField("scene", &IsIngredientAsset::scene)
        .build();
}

CUBOS_REFLECT_IMPL(demo::IsIngredient)
{
    return cubos::core::ecs::TypeBuilder<IsIngredient>("demo::IsIngredient").tree().build();
}

CUBOS_REFLECT_IMPL(demo::Requires)
{
    return cubos::core::ecs::TypeBuilder<Requires>("demo::Requires").withField("amount", &Requires::amount).build();
}

CUBOS_REFLECT_IMPL(demo::ConstructedFrom)
{
    return cubos::core::ecs::TypeBuilder<ConstructedFrom>("demo::ConstructedFrom").tree().build();
}

static int getStackSize(Entity bottom, Query<Entity, const ChildOf&>& children)
{
    int height = 1;
    while (auto match = children.pin(1, bottom).first())
    {
        auto [next, childOf] = *match;
        bottom = next;
        height++;
    }
    return height;
}

static int findAmountInStack(Entity bottom, Entity ingredient, Query<Entity, const ChildOf&>& children,
                             Query<Entity, const demo::IsIngredient&, Entity> isIngredient)
{
    int amount = 0;
    if (isIngredient.at(bottom, ingredient))
    {
        amount += 1;
    }
    for (auto match : children.pin(1, bottom))
    {
        auto [next, childOf] = match;
        amount += findAmountInStack(next, ingredient, children, isIngredient);
    }
    return amount;
}

void demo::hammerPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(transformPlugin);
    cubos.depends(interactionPlugin);
    cubos.depends(objectPlugin);
    cubos.depends(holdablePlugin);

    cubos.component<Hammer>();
    cubos.component<Recipe>();
    cubos.component<Ingredient>();
    cubos.component<IsIngredientAsset>();

    cubos.relation<IsIngredient>();
    cubos.relation<Requires>();
    cubos.relation<ConstructedFrom>();

    cubos.observer("replace IsIngredientAsset by IsIngredient")
        .onAdd<IsIngredientAsset>()
        .call([](Commands cmds, Query<Entity, const IsIngredientAsset&> isIngredientAssets,
                 Query<Entity, const Ingredient&> ingredients) {
            for (auto [ent, isIngredientAsset] : isIngredientAssets)
            {
                bool found = false;

                for (auto [ingredientEnt, ingredient] : ingredients)
                {
                    if (ingredient.scene == isIngredientAsset.scene)
                    {
                        cmds.relate(ent, ingredientEnt, IsIngredient{});
                        cmds.remove<IsIngredientAsset>(ent);
                        found = true;
                        break;
                    }
                }

                CUBOS_ASSERT(found, "No ingredient type {} found for IsIngredientAsset", isIngredientAsset.scene);
            }
        });

    cubos.observer("handle Hammer interactions with ingredients")
        .onAdd<Interaction>(0)
        .with<Object>(0)
        .related<IsIngredient>(0, 1)
        .entity(1)
        .related<ChildOf>(0, 2)
        .entity(2)
        .call([](Commands cmds, Assets& assets,
                 Query<Entity, const Interaction&, const Object&, Entity, Entity> interactions,
                 Query<Entity, const Recipe&, const Requires&, Entity> recipes,
                 Query<const Hammer&, const ChildOf&> hammers, Query<Entity, const ChildOf&> children,
                 Query<Entity, const demo::IsIngredient&, Entity> isIngredient) {
            for (auto [bottomEnt, interaction, object, ingredientEnt, mapEnt] : interactions)
            {
                if (!hammers.pin(1, interaction.entity).empty())
                {
                    // Find a recipe that matches the ingredients in the pile.
                    for (auto [recipeEnt, recipe, _1, _2] : recipes.pin(1, ingredientEnt))
                    {
                        bool valid = true;
                        auto totalAmount = getStackSize(bottomEnt, children);

                        for (auto [_3, _4, require, ent] : recipes.pin(0, recipeEnt))
                        {
                            auto amount = findAmountInStack(bottomEnt, ent, children, isIngredient);
                            if (amount != require.amount)
                            {
                                valid = false;
                                break;
                            }
                            totalAmount -= amount;
                        }

                        if (valid && totalAmount == 0)
                        {
                            cmds.destroy(bottomEnt);
                            auto resultEnt = cmds.spawn(assets.read(recipe.result)->blueprint).entity(recipe.root);
                            cmds.relate(resultEnt, mapEnt, ChildOf{});
                            cmds.relate(resultEnt, recipeEnt, ConstructedFrom{});
                            cmds.add(resultEnt, Object{.position = object.position, .size = object.size});
                            break;
                        }
                    }
                }
            }
        });

    cubos.observer("handle Hammer interactions with constructions")
        .onAdd<Interaction>(0)
        .with<Object>(0)
        .related<ConstructedFrom>(0, 1)
        .entity(1)
        .related<ChildOf>(0, 2)
        .entity(2)
        .call([](Commands cmds, Assets& assets,
                 Query<Entity, const Interaction&, const Object&, Entity, Entity> interactions,
                 Query<const Requires&, const Ingredient&> ingredients, Query<const Hammer&, const ChildOf&> hammers,
                 Query<const Holdable&, const ChildOf&> stacked) {
            for (auto [ent, interaction, object, recipeEnt, mapEnt] : interactions)
            {
                if (!hammers.pin(1, interaction.entity).empty())
                {
                    // If the interaction is with a stack of ingredients, do nothing.
                    if (!stacked.pin(1, ent).empty())
                    {
                        continue;
                    }

                    cmds.destroy(ent);

                    // Spawn pile of ingredients.
                    Entity topEnt = mapEnt;
                    for (auto [require, ingredient] : ingredients.pin(0, recipeEnt))
                    {
                        for (int i = 0; i < require.amount; ++i)
                        {
                            auto dropEnt = cmds.spawn(assets.read(ingredient.scene)->blueprint).entity(ingredient.root);
                            cmds.relate(dropEnt, topEnt, ChildOf{});

                            if (topEnt == mapEnt)
                            {
                                cmds.add(dropEnt, Object{.position = object.position, .size = object.size});
                            }
                            else
                            {
                                auto rotation = ((float)rand() / (float)RAND_MAX) * 180.0F;
                                cmds.add(dropEnt, Position{{0.0F, 1.0F, 0.0F}})
                                    .add(dropEnt,
                                         Rotation{glm::angleAxis(glm::radians(rotation), glm::vec3(0.0F, 1.0F, 0.0F))});
                            }

                            topEnt = dropEnt;
                        }
                    }
                }
            }
        });
}
