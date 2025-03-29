#include "plugin.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

// TODO: Get rid of DEFINE
#define GRID_SIZE 16

namespace demo
{
    std::optional<int> Waves::fetch(int x, int y) const
    {
        if (x < 0 || x > GRID_SIZE - 1)
            return std::nullopt;
        if (y < 0 || y > GRID_SIZE - 1)
            return std::nullopt;
        return state[y][x];
    }

    void Waves::modify(int x, int y, int value)
    {
        if (x < 0 || x > GRID_SIZE - 1)
            return;
        if (y < 0 || y > GRID_SIZE - 1)
            return;
        stateNext[y][x] = value;
    }

    void Waves::step(int value, int x, int y)
    {
        // Get the avg. of values on the left, and set our value to it
        const auto leftValues = {
            fetch(x - 1, y - 1),
            fetch(x - 1, y),
            fetch(x - 1, y + 1),
        };
        int sum = 0, num = 0;
        for (auto& optValue : leftValues)
        {
            if (!optValue)
                continue;
            ++num;
            sum += *optValue;
        }
        if (num == 0)
        {
            // We're on the edge, decrement value each tick
            modify(x, y, std::max(value - 1, 0));
        }
        else
        {
            int result = std::ceil((float)sum / num);

            // Only allow modifications if the result is higher than terrain.
            // Otherwise, set it to 0
            if (result < terrain[y][x])
                result = 0;

            modify(x, y, result);
        }
    }

    void Waves::iteration()
    {
        for (int y = 0; y < state.size(); ++y)
        {
            for (int x = 0; x < state[y].size(); ++x)
            {
                step(state[y][x], x, y);
            }
        }

        print(state);
        // print(stateNext);

        // Copy next to curr, and clear next buff
        for (int y = 0; y < stateNext.size(); ++y)
        {
            for (int x = 0; x < stateNext[y].size(); ++x)
            {
                state[y][x] = stateNext[y][x];
                stateNext[y][x] = 0;
            }
        }
    }

    std::string Waves::convertValueToASCII(int value)
    {
        switch (value)
        {
        case 5:
            return "█";
        case 4:
            return "▆";
        case 3:
            return "▄";
        case 2:
            return "▂";
        case 1:
            return "▁";
        case 0:
            return ".";

        default:
            return "?";
        }
    }

    void Waves::print(std::vector<std::vector<int>>& state)
    {
        CUBOS_INFO("Automata state:");
        for (auto& v : state)
        {
            for (bool firstPrint = true; auto& i : v)
            {
                if (!firstPrint)
                    std::cout << " ";
                std::cout << convertValueToASCII(i);
                firstPrint = false;
            }
            std::cout << std::endl;
        }
    }
} // namespace demo

CUBOS_REFLECT_IMPL(demo::Waves)
{
    return cubos::core::ecs::TypeBuilder<Waves>("demo::Waves")
        .withField("accumDeltaTime", &Waves::accumDeltaTime)
        .withField("iter", &Waves::iter)
        .build();
}

demo::Waves* getFirst(Query<demo::Waves&>& q)
{
    auto wavesOpt = q.first();
    if (!wavesOpt)
        return nullptr;

    return &std::get<0>(wavesOpt.value());
}

void demo::wavesPlugin(Cubos& cubos)
{
    cubos.component<Waves>();

    cubos.observer("initialize waves component").onAdd<Waves>().call([](Query<Waves&> q) {
        auto waves = getFirst(q);
        if (!waves)
            return;

        waves->state.resize(GRID_SIZE);
        waves->stateNext.resize(GRID_SIZE);
        waves->terrain.resize(GRID_SIZE);
        for (auto& v : waves->state)
        {
            v.resize(GRID_SIZE);
        }
        for (auto& v : waves->stateNext)
        {
            v.resize(GRID_SIZE);
        }
        for (auto& v : waves->terrain)
        {
            v.resize(GRID_SIZE);
        }

        // for( auto &v : waves->state ) {
        //     for( int& i : v ) {
        //         i = (rand() % 5) + 1;
        //     }
        // }

        // waves->state[3][2] = 5;
        // waves->state[4][2] = 5;
        // waves->state[5][2] = 5;
        // waves->state[6][2] = 5;
    });

    cubos.system("update waves calculation").call([](const DeltaTime& dt, Query<Waves&> q) {
        auto waves = getFirst(q);
        if (!waves)
            return;

        waves->accumDeltaTime += dt.value();
        if (waves->accumDeltaTime >= 0.6f)
        {
            waves->accumDeltaTime = 0.0f;

            float t = (float)waves->iter * 0.5;
            float waveRatio = (sin(t) + 1.0) * 0.5;
            int waveHeight = static_cast<int>(waveRatio * 10);
            for (int i = 0; i < GRID_SIZE; ++i)
            {
                waves->state[i][0] = waveHeight;
            }

            // waves->terrain[8][10] = 5;
            // waves->terrain[7][10] = 5;
            // waves->terrain[6][10] = 5;
            // waves->terrain[9][10] = 5;
            // waves->terrain[8][11] = 5;
            // waves->terrain[7][11] = 5;

            CUBOS_INFO("Iteration: {}", waves->iter++);
            waves->iteration();
        }
    });
}