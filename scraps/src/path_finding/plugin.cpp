#include "plugin.hpp"
#include "../tile_map/plugin.hpp"
#include "../health/plugin.hpp"

#include <set>
#include <queue>
#include <algorithm>

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/core/reflection/external/vector.hpp>
#include <cubos/core/thread/pool.hpp>

#include <cubos/engine/transform/plugin.hpp>

using namespace cubos::engine;
using cubos::core::thread::ThreadPool;

CUBOS_REFLECT_IMPL(demo::PathTask)
{
    return cubos::core::ecs::TypeBuilder<PathTask>("demo::PathTask")
        .withField("from", &PathTask::from)
        .withField("team", &PathTask::team)
        .withField("hpPenalization", &PathTask::hpPenalization)
        .build();
}

CUBOS_REFLECT_IMPL(demo::Target)
{
    return cubos::core::ecs::TypeBuilder<Target>("demo::Target").withField("cost", &Target::cost).build();
}

CUBOS_REFLECT_IMPL(demo::Path)
{
    return cubos::core::ecs::TypeBuilder<Path>("demo::Path").withField("path", &Path::path).build();
}

namespace
{
    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ThreadPool pool;

        State(std::size_t threads) : pool(threads)
        {
        }
    };

    struct Node
    {
        bool target{false};
        float cost{0};
        float f{INFINITY};
        float g{INFINITY};
        float h{INFINITY};
        glm::ivec2 parent{};
    };
} // namespace

static glm::ivec2 aStar(std::vector<std::vector<Node>>& nodes, glm::ivec2 from)
{
    glm::ivec2 directions[] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    auto heuristic = [](glm::ivec2 p) { return p.x < 22 ? static_cast<float>(22 - p.x) : 0.0F; };

    std::priority_queue<glm::ivec2, std::vector<glm::ivec2>, std::function<bool(glm::ivec2, glm::ivec2)>> openSet(
        [&nodes](glm::ivec2 a, glm::ivec2 b) { return nodes[a.y][a.x].f > nodes[b.y][b.x].f; });
    std::set<std::pair<int, int>> closedSet{};
    openSet.push(from);

    nodes[from.y][from.x].f = 0.0F;
    nodes[from.y][from.x].g = 0.0F;
    nodes[from.y][from.x].h = heuristic(from);

    while (!openSet.empty())
    {
        auto current = openSet.top();

        if (nodes[current.y][current.x].target)
            return current;

        openSet.pop();
        closedSet.insert({current.x, current.y});

        for (auto direction : directions)
        {
            auto next = current + direction;
            if (next.x < 0 || next.y < 0 || next.x >= nodes.size() || next.y >= nodes.size())
                continue;

            if (closedSet.find({next.x, next.y}) != closedSet.end())
                continue;

            auto g = nodes[current.y][current.x].g + nodes[next.y][next.x].cost;
            auto h = heuristic(next);
            auto f = g + h;

            if (f < nodes[next.y][next.x].f)
            {
                nodes[next.y][next.x].f = f;
                nodes[next.y][next.x].g = g;
                nodes[next.y][next.x].h = h;
                nodes[next.y][next.x].parent = current;
                openSet.push(next);
            }
        }
    }

    return {-1, -1};
}

void demo::pathFindingPlugin(Cubos& cubos)
{
    cubos.depends(transformPlugin);
    cubos.depends(tileMapPlugin);
    cubos.depends(healthPlugin);

    cubos.resource<State>(4);

    cubos.component<PathTask>();
    cubos.component<Target>();
    cubos.component<Path>();

    cubos.observer("start PathTask")
        .onAdd<PathTask>()
        .call([](State& state, Query<const PathTask&, const ChildOf&, const TileMap&> query,
                 Query<const Health&, Opt<const Target&>> healths) {
            for (auto [pathTask, childOf, tileMap] : query)
            {
                std::vector<std::vector<Node>> nodes{tileMap.entities.size(),
                                                     std::vector<Node>(tileMap.entities.size())};

                for (std::size_t y = 0; y < tileMap.entities.size(); ++y)
                {
                    for (std::size_t x = 0; x < tileMap.entities.size(); ++x)
                    {
                        if (tileMap.entities[y][x].isNull())
                        {
                            nodes[y][x].cost = 1.0F;
                            nodes[y][x].target = false;
                        }
                        else if (!healths.at(tileMap.entities[y][x]).contains())
                        {
                            // Indestructible entity.
                            nodes[y][x].cost = INFINITY;
                            nodes[y][x].target = false;
                        }
                        else
                        {
                            auto [health, target] = *healths.at(tileMap.entities[y][x]);
                            if (health.team == pathTask.team)
                            {
                                // We can't destroy our own entities.
                                nodes[y][x].cost = INFINITY;
                                nodes[y][x].target = false;
                            }
                            else
                            {
                                if (target)
                                {
                                    nodes[y][x].cost = 1.0F + target->cost;
                                    nodes[y][x].target = true;
                                }
                                else
                                {
                                    nodes[y][x].cost = 1.0F + pathTask.hpPenalization * health.hp;
                                    nodes[y][x].target = false;
                                }
                            }
                        }
                    }
                }

                state.pool.addTask([from = pathTask.from, task = pathTask.task, nodes = std::move(nodes)]() mutable {
                    std::vector<glm::ivec2> path{};
                    if (auto position = aStar(nodes, from); position != glm::ivec2{-1, -1})
                    {
                        auto current = position;
                        while (current != from)
                        {
                            path.push_back(current);
                            current = nodes[current.y][current.x].parent;
                        }
                        path.push_back(from);
                    }
                    task.finish(std::move(path));
                });
            }
        });

    cubos.system("poll PathTask").call([](Commands cmds, State& state, Query<Entity, PathTask&> query) {
        for (auto [ent, pathTask] : query)
        {
            if (pathTask.task.isDone())
            {
                cmds.add(ent, Path{pathTask.task.result()});
                cmds.remove<PathTask>(ent);
            }
        }
    });
}
