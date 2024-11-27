#pragma once

#include <cubos/core/ecs/system/arguments/event/pipe.hpp>
#include <cubos/engine/prelude.hpp>

namespace airships::common
{
    template <typename T> class PacketPipe
    {
    public:
        template <unsigned int M = DEFAULT_FILTER_MASK>
        std::unique_ptr<cubos::engine::EventReader<T, M>> createReader(size_t& index)
        {
            return std::make_unique<cubos::engine::EventReader<T, M>>(mPipe, index);
        }

        std::unique_ptr<cubos::engine::EventWriter<T>> createWriter()
        {
            return std::make_unique<cubos::engine::EventWriter<T>>(mPipe);
        }

    private:
        cubos::core::ecs::EventPipe<T> mPipe;
    };
} // namespace airships::common
