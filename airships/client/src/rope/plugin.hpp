#pragma once

#include <glm/vec3.hpp>

#include <cubos/core/gl/render_device.hpp>

#include <cubos/engine/prelude.hpp>

namespace airships::client
{
    /// @brief Relation which renders a rope between two entities.
    struct Rope
    {
        CUBOS_REFLECT;

        /// @brief Edge size of the square cross-section of the rope.
        float thickness;

        /// @brief Color of the rope.
        glm::vec3 color;
    };

    /// @brief Component to indicate that ropes should be drawn to this target.
    struct RopeRasterizer
    {
        CUBOS_REFLECT;

        /// @brief Framebuffer used by the rasterizer to render to the GBuffer.
        cubos::core::gl::Framebuffer framebuffer{nullptr};

        /// @brief GBuffer's position texture in the current framebuffer.
        cubos::core::gl::Texture2D position{nullptr};

        /// @brief GBuffer's normal texture in the current framebuffer.
        cubos::core::gl::Texture2D normal{nullptr};

        /// @brief GBuffer's albedo texture in the current framebuffer.
        cubos::core::gl::Texture2D albedo{nullptr};

        /// @brief RenderDepth texture in the current framebuffer.
        cubos::core::gl::Texture2D depth{nullptr};
    };

    void ropePlugin(cubos::engine::Cubos& cubos);
} // namespace airships::client
