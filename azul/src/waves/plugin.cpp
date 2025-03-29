#include "plugin.hpp"

#include <iostream>
#include <optional>

#include <cubos/core/ecs/reflection.hpp>

using namespace cubos::engine;

// TODO: Get rid of DEFINE
#define GRID_SIZE 16

namespace demo
{
    struct Waves
    {
        CUBOS_REFLECT;

        std::vector<std::vector<int>> state, stateNext;

        float accumDeltaTime = 0;
        int iter = 0;

        std::optional<int> fetch(int x, int y) const {
            if( x < 0 || x > GRID_SIZE-1) return std::nullopt;
            if( y < 0 || y > GRID_SIZE-1) return std::nullopt;

            return stateNext[y][x];
        }

        void modify(int x, int y, int value) {
            if( x < 0 || x > GRID_SIZE-1) return;
            if( y < 0 || y > GRID_SIZE-1) return;

            stateNext[y][x] = value;
        }

        void step(int value, int x, int y) {
            // If no wave, no behave
            if(value <= 0) return;

            // If we are at the highest, we move to the right, and affect next values
            if(value == 5) {
                modify(x+1, y, value);
                modify(x+2, y, value - 2);
                modify(x+3, y, value - 4);
            }
            // ...and, if our value at modified it at 5, we were affected
            // by a wave, so don't self modify
            if(auto v = fetch(x, y); v && *v == 5) return;
            // otherwise, decrease intensity on our spot
            modify(x, y, value-1);
        }

        void iteration() {
            for(int y = 0; y < state.size(); ++y) {
                for(int x = 0; x < state[y].size(); ++x) {
                    step(state[y][x], x, y);
                }
            }

            print(state);
            //print(stateNext);
            // Copy next to curr, and clear next buff

            for(int y = 0; y < stateNext.size(); ++y) {
                for(int x = 0; x < stateNext[y].size(); ++x) {
                    state[y][x] = stateNext[y][x];
                    stateNext[y][x] = 0;
                }
            }

            //std::swap(state, stateNext);
        }

        std::string convertValueToASCII(int value) {
            switch (value)
            {
            case 5: return "█";
            case 4: return "▆";
            case 3: return "▄";
            case 2: return "▂";
            case 1: return "▁";
            case 0: return ".";
            
            default: return "?";
            }
        }

        void print(std::vector<std::vector<int>>& state)
        {
            CUBOS_INFO("Automata state:");
            for(auto& v : state){
                for(bool firstPrint = true; auto& i : v) {
                    if(!firstPrint)
                        std::cout << " ";
                    std::cout << convertValueToASCII(i);
                    firstPrint = false;
                }
                std::cout << std::endl;
            }
        }
    };
}

CUBOS_REFLECT_IMPL(demo::Waves)
{
    return cubos::core::ecs::TypeBuilder<Waves>("demo::Waves")
        .withField("accumDeltaTime", &Waves::accumDeltaTime)
        .withField("iter", &Waves::iter)
        .build();
}

demo::Waves* getFirst(Query<demo::Waves&> &q) {
    auto wavesOpt = q.first();
    if( !wavesOpt ) return nullptr;

    return &std::get<0>(wavesOpt.value());
}

void demo::wavesPlugin(Cubos& cubos)
{
    cubos.component<Waves>();

    cubos.observer("initialize waves component")
        .onAdd<Waves>()
        .call([](Query<Waves&> q) {
            auto waves = getFirst(q);
            if(!waves) return;

            waves->state.resize(GRID_SIZE);
            waves->stateNext.resize(GRID_SIZE);
            for( auto& v : waves->state ) {
                v.resize(GRID_SIZE);
            }
            for( auto& v : waves->stateNext ) {
                v.resize(GRID_SIZE);
            }

            for( auto &v : waves->state ) {
                for( int& i : v ) {
                    i = (rand() % 5) + 1;
                }
            }

            //waves->state[2][2] = 5;
            //waves->state[3][2] = 5;
            //waves->state[4][2] = 5;
            //waves->state[5][2] = 5;
            //waves->state[6][2] = 5;
        });

    cubos.system("update waves calculation")
        .call([](const DeltaTime& dt, Query<Waves&> q){
            auto waves = getFirst(q);
            if(!waves) return;

            waves->accumDeltaTime += dt.value();
            if(waves->accumDeltaTime >= 0.6f) {
                waves->accumDeltaTime = 0.0f;

                CUBOS_INFO("Iteration: {}", waves->iter++);
                waves->iteration();
            }
        });
}