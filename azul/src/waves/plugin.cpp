#include "plugin.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <optional>
#include <glm/glm.hpp>

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

namespace demo
{
    std::optional<float> Waves::fetch(int x, int y) const
    {
        if (x < 0 || x > state[0].size() - 1)
            return std::nullopt;
        if (y < 0 || y > state.size() - 1)
            return std::nullopt;
        return state[y][x];
    }

    void Waves::modify(int x, int y, float value)
    {
        if (x < 0 || x > state[0].size() - 1)
            return;
        if (y < 0 || y > state.size() - 1)
            return;
        stateNext[y][x] = value;
    }

    void Waves::step(float value, int x, int y)
    {
        float result = -INFINITY;

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

        if (result == -INFINITY)
        {
            modify(x, y, value);
            return;
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
            result = seaLevel;

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

        float t = (float)(iter++) * waveFrequency;
        float waveHeight = seaLevel + waveAmplitude * sin(t);
        for (int y = 0; y < state.size(); ++y)
        {
            stateNext[y][0] = waveHeight;
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
        .withField("lerpFactor", &Waves::lerpFactor)
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
        waves->actual.resize(waves->terrain.size());
        for (auto& v : waves->state)
        {
            v.resize(waves->terrain[0].size(), waves->seaLevel);
        }
        for (auto& v : waves->stateNext)
        {
            v.resize(waves->terrain[0].size(), waves->seaLevel);
        }
        for (auto& v : waves->actual)
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
        }

        for (int y = 0; y < waves->state.size(); ++y)
        {
            for (int x = 0; x < waves->state.size(); ++x)
            {
                waves->actual[y][x] = glm::mix(waves->actual[y][x], waves->state[y][x],
                                                 1 - glm::pow(1.0 - waves->lerpFactor, dt.value()));
            }
        }
    });
}