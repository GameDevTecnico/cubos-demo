#pragma once

#include <cubos/core/reflection/reflect.hpp>
#include <cubos/core/reflection/external/map.hpp>
#include <cubos/core/reflection/external/glm.hpp>
#include <cubos/engine/prelude.hpp>
#include <cubos/engine/api.hpp>
#include <map>
#include <glm/vec3.hpp>

namespace airships::client
{
    struct ChunkInfo
    {
        CUBOS_REFLECT;

        int chunkSize = 75;
        int minRnd = -25.0;
        int maxRnd = 20.0;
        std::map<int, glm::vec3> chunksTaken;
    };

    struct RandomPosition 
    {
        CUBOS_REFLECT;
        glm::vec3 startingPos = {0.0F, 0.0F, 0.0F};
    };

    /// @brief Plugin that gives entities a random position but not overlapping with others
    /// @param cubos
    void randomPositionPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client