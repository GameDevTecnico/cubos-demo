#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/api.hpp>
#include <map>

namespace airships::client
{
    struct ChunkInfo
    {
        CUBOS_REFLECT;

        int chunkSize = 50;
        int minRnd = -5.0;
        int maxRnd = 5.0;
        std::map<int, glm::vec3> chunksTaken;
    };

    struct RandomPosition 
    {
        CUBOS_REFLECT;
    };

    /// @brief plugin that gives entities a random position but not overlapping with others
    /// @param cubos
    void randompositionPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client