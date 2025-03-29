#include "plugin.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

namespace demo
{
    std::optional<int> Waves::fetch(int x, int y) const
    {
        if (x < 0 || x > state[0].size() - 1)
            return std::nullopt;
        if (y < 0 || y > state.size() - 1)
            return std::nullopt;
        return state[y][x];
    }

    void Waves::modify(int x, int y, int value)
    {
        if (x < 0 || x > state[0].size() - 1)
            return;
        if (y < 0 || y > state.size() - 1)
            return;
        stateNext[y][x] = value;
    }

    void Waves::step(int value, int x, int y)
    {
        int result = INT_MIN;

        // Get the avg. of values on the left, and set our value to it
        const auto leftValues = {
            fetch(x - 1, y - 1),
            fetch(x - 1, y),
            fetch(x - 1, y + 1),
        };
        for (auto& optValue : leftValues)
        {
            if (!optValue)
                continue;
            result = std::max(result, *optValue);
        }

        if (result == INT_MIN)
        {
            result = std::max(value - 1, 0);
        }

        // int sum = 0, num = 0;
        // for (auto& optValue : leftValues)
        // {
        //     if (!optValue)
        //         continue;
        //     ++num;
        //     sum += *optValue;
        // }

        // int result;
        // if (num == 0)
        // {
        //     // We're on the edge, decrement value each tick
        //     result = std::max(value - 1, 0);
        // }
        // else
        // {
        //     num += 1;
        //     result = std::ceil((float)sum / num);
        // }

        // Only allow modifications if the result is higher than terrain.
        // Otherwise, set it to 0
        if (result < terrain[y][x])
            result = 0;

        modify(x, y, result);
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
                std::cout << i;
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
        .withField("updateInterval", &Waves::updateInterval)
        .withField("waveAmplitude", &Waves::waveAmplitude)
        .withField("waveFrequency", &Waves::waveFrequency)
        .withField("seaLevel", &Waves::seaLevel)
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

        waves->state.resize(waves->terrain.size());
        waves->stateNext.resize(waves->terrain.size());
        for (auto& v : waves->state)
        {
            v.resize(waves->terrain[0].size(), waves->seaLevel);
        }
        for (auto& v : waves->stateNext)
        {
            v.resize(waves->terrain[0].size(), waves->seaLevel);
        }
    });

    cubos.system("update waves calculation").call([](const DeltaTime& dt, Query<Waves&> q) {
        auto waves = getFirst(q);
        if (!waves)
            return;

        waves->accumDeltaTime += dt.value();
        while (waves->accumDeltaTime >= waves->updateInterval)
        {
            waves->accumDeltaTime -= waves->updateInterval;

            waves->iteration();

            float t = (float)(waves->iter++) * waves->waveFrequency;
            int waveHeight = static_cast<int>(roundf(waves->seaLevel + waves->waveAmplitude * sin(t)));
            for (int i = 0; i < waves->state.size(); ++i)
            {
                waves->state[i][0] = waveHeight;
            }
        }
    });
}