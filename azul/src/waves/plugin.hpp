#pragma once

#include <cubos/engine/prelude.hpp>

#include <optional>
#include <vector>

namespace demo
{
    struct Waves
    {
        CUBOS_REFLECT;

        std::vector<std::vector<int>> state, stateNext;
        std::vector<std::vector<int>> terrain;

        float accumDeltaTime = 0;
        int iter = 0;

        std::optional<int> fetch(int x, int y) const;
        void modify(int x, int y, int value);
        void step(int value, int x, int y);
        void iteration();
        std::string convertValueToASCII(int value);
        void print(std::vector<std::vector<int>>& state);
    };

    void wavesPlugin(cubos::engine::Cubos& cubos);
} // namespace demo