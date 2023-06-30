#include "follow.hpp"

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/renderer/plugin.hpp>
#include <cubos/engine/renderer/light.hpp>

#include <unordered_map>
#include "components.hpp"

using cubos::core::ecs::Query;
using cubos::core::ecs::Read;
using cubos::core::ecs::Write;
using cubos::core::ecs::Entity;
using namespace cubos::engine;

/// @brief Used just to store the entities to be followed
struct FollowInfo
{
    std::unordered_map<Entity, std::tuple<Position, Rotation>> parents;
    //Query<Read<Car>, Read<cubos::engine::Position>, Read<cubos::engine::Rotation>> parents;
};

static void cameraFollowSystem(Query<Read<Camera>, Write<Position>, Write<Rotation>, Write<FollowEntity>> query, Read<FollowInfo> followInfo) {
    for (auto [entity, camera, position, rotation, followEntity] : query)
    {
        //auto parent = followInfo->parents.operator[](followEntity->entityToFollow).value();
        //auto [parentEntity, car, parentPosition, parentRotation] = parent;
        auto parent = followInfo->parents.find(followEntity->entityToFollow);
        if (parent == followInfo->parents.end()) {
            continue;
        }
        auto [parentPosition, parentRotation] = parent->second;
        rotation->quat = parentRotation.quat *
                         glm::angleAxis(3.1415f, glm::vec3(0.0F, 1.0F, 0.0F)) *
                         glm::angleAxis(-0.2618f, glm::vec3(1.0f, 0.0f, 0.0f));
        position->vec = parentPosition.vec +
                        (glm::vec3(0.0f, 1.0f, 0.0f) * followEntity->positionOffset) +
                        (glm::normalize(rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f)) * 60.0f);
    }
}

static void lightFollowSystem(Query<Read<SpotLight>, Write<Position>, Write<Rotation>, Write<FollowEntity>> query, Read<FollowInfo> followInfo)
{
    for (auto [entity, light, position, rotation, followEntity] : query)
    {
        //auto parent = followInfo->parents.operator[](followEntity->entityToFollow).value();
        //if (parent != nullptr)
        //auto [parentEntity, car, parentPosition, parentRotation] = parent;
        auto parent = followInfo->parents.find(followEntity->entityToFollow);
        if (parent == followInfo->parents.end()) {
            continue;
        }
        auto [parentPosition, parentRotation] = parent->second;

        rotation->quat = parentRotation.quat *
                         followEntity->rotationOffset;

        position->vec = parentPosition.vec +
                        (glm::vec3(0.0f, 1.0f, 0.0f) * followEntity->positionOffset) +
                        (glm::normalize(rotation->quat * glm::vec3(0.0f, 0.0f, 1.0f)) * 20.0f);
    }
}
static void updateParent(Query<Read<Car>, Read<Position>, Read<Rotation>> query, Write<FollowInfo> followInfo) 
{
    for (auto [entity, car, position, rotation] : query) {
        followInfo->parents.insert_or_assign(entity, std::make_tuple(*position, *rotation));
    }
    
    //followInfo->parents = query;
}

void followPlugin(Cubos& cubos)
{
    cubos.addResource<FollowInfo>();

    cubos.addComponent<FollowEntity>();

    cubos.system(updateParent).tagged("follow.update").afterTag("car.move");
    //cubos.system(updateFollow).tagged("follow.update").afterTag("car.move");
    cubos.system(cameraFollowSystem).afterTag("follow.update");
    cubos.system(lightFollowSystem).afterTag("follow.update");
}