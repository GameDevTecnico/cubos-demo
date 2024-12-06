#include <cubos/engine/prelude.hpp>

namespace airships::server
{
    extern cubos::engine::Tag acceptConnectionTag;
    extern cubos::engine::Tag networkStartTag;
    extern cubos::engine::Tag inPacketHandlerTag;
    extern cubos::engine::Tag outPacketHandlerTag;
    extern cubos::engine::Tag networkEndTag;

    void networkPlugin(cubos::engine::Cubos& cubos);
} // namespace airships::server
