#pragma once

#include <cubos/engine/prelude.hpp>

#include <optional>
#include <vector>

namespace demo
{
    struct Waves
    {
        CUBOS_REFLECT;

        std::vector<std::vector<float>> state, stateNext;
        std::vector<std::vector<float>> actual;
        std::vector<std::vector<int>> terrain;

        float updateInterval = 0.4F;
        float waveFrequency = 0.5F;
        float waveAmplitude = 2.0F;
        float seaLevel = 3.0F;
        float lerpFactor{0.5F};

        float accumDeltaTime = 0;
        int iter = 0;

        std::optional<float> fetch(int x, int y) const;
        void modify(int x, int y, float value);
        void step(float value, int x, int y);
        void iteration();
    };

    void wavesPlugin(cubos::engine::Cubos& cubos);
} // namespace demo