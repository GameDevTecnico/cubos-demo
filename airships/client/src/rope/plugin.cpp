#include "plugin.hpp"
#include "../hide/plugin.hpp"

#include <cubos/core/ecs/reflection.hpp>
#include <cubos/core/reflection/external/primitives.hpp>
#include <cubos/core/reflection/external/glm.hpp>

#include <cubos/engine/transform/plugin.hpp>
#include <cubos/engine/render/g_buffer/g_buffer.hpp>
#include <cubos/engine/render/g_buffer/plugin.hpp>
#include <cubos/engine/render/depth/plugin.hpp>
#include <cubos/engine/render/depth/depth.hpp>
#include <cubos/engine/render/camera/plugin.hpp>
#include <cubos/engine/render/camera/camera.hpp>
#include <cubos/engine/render/camera/draws_to.hpp>
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>
#include <cubos/engine/render/shader/plugin.hpp>

using namespace cubos::core::gl;
using namespace cubos::engine;

using cubos::core::io::Window;

CUBOS_REFLECT_IMPL(airships::client::Rope)
{
    return cubos::core::ecs::TypeBuilder<Rope>("airships::client::Rope")
        .withField("thickness", &Rope::thickness)
        .withField("color", &Rope::color)
        .symmetric()
        .build();
}

CUBOS_REFLECT_IMPL(airships::client::RopeRasterizer)
{
    return cubos::core::ecs::TypeBuilder<RopeRasterizer>("airships::client::RopeRasterizer").build();
}

namespace
{
    // Holds the data sent per scene to the GPU.
    struct PerScene
    {
        glm::mat4 viewProj;
    };

    // Holds the data sent per rope to the GPU.
    struct PerRope
    {
        glm::mat4 model;
        glm::vec4 color;
    };

    struct State
    {
        CUBOS_ANONYMOUS_REFLECT(State);

        ShaderPipeline pipeline;
        ShaderBindingPoint perSceneBP;
        ShaderBindingPoint perRopeBP;

        RasterState rasterState;
        DepthStencilState depthStencilState;

        VertexArray vertexArray;

        ConstantBuffer perSceneCB;
        ConstantBuffer perRopeCB;

        State(RenderDevice& renderDevice, const ShaderPipeline& pipeline) : pipeline(pipeline)
        {
            perSceneBP = pipeline->getBindingPoint("PerScene");
            perRopeBP = pipeline->getBindingPoint("PerRope");
            CUBOS_ASSERT(perSceneBP && perRopeBP, "PerScene and PerRope binding points must exist");

            rasterState = renderDevice.createRasterState({
                .cullEnabled = true,
                .cullFace = Face::Back,
                .frontFace = Winding::CCW,
            });

            depthStencilState = renderDevice.createDepthStencilState({
                .depth = {.enabled = true, .writeEnabled = true},
            });

            float buf[] = {
                // -X face
                -0.5F, -0.5F, -0.5F, -1.0F, 0.0F, 0.0F, //
                -0.5F, +0.5F, -0.5F, -1.0F, 0.0F, 0.0F, //
                -0.5F, +0.5F, +0.5F, -1.0F, 0.0F, 0.0F, //
                -0.5F, +0.5F, +0.5F, -1.0F, 0.0F, 0.0F, //
                -0.5F, -0.5F, +0.5F, -1.0F, 0.0F, 0.0F, //
                -0.5F, -0.5F, -0.5F, -1.0F, 0.0F, 0.0F, //

                // +X face
                +0.5F, +0.5F, -0.5F, +1.0F, 0.0F, 0.0F, //
                +0.5F, -0.5F, -0.5F, +1.0F, 0.0F, 0.0F, //
                +0.5F, +0.5F, +0.5F, +1.0F, 0.0F, 0.0F, //
                +0.5F, -0.5F, +0.5F, +1.0F, 0.0F, 0.0F, //
                +0.5F, +0.5F, +0.5F, +1.0F, 0.0F, 0.0F, //
                +0.5F, -0.5F, -0.5F, +1.0F, 0.0F, 0.0F, //

                // -Y face
                -0.5F, -0.5F, -0.5F, 0.0F, -1.0F, 0.0F, //
                +0.5F, -0.5F, +0.5F, 0.0F, -1.0F, 0.0F, //
                +0.5F, -0.5F, -0.5F, 0.0F, -1.0F, 0.0F, //
                +0.5F, -0.5F, +0.5F, 0.0F, -1.0F, 0.0F, //
                -0.5F, -0.5F, -0.5F, 0.0F, -1.0F, 0.0F, //
                -0.5F, -0.5F, +0.5F, 0.0F, -1.0F, 0.0F, //

                // +Y face
                -0.5F, +0.5F, -0.5F, 0.0F, +1.0F, 0.0F, //
                +0.5F, +0.5F, -0.5F, 0.0F, +1.0F, 0.0F, //
                +0.5F, +0.5F, +0.5F, 0.0F, +1.0F, 0.0F, //
                +0.5F, +0.5F, +0.5F, 0.0F, +1.0F, 0.0F, //
                -0.5F, +0.5F, +0.5F, 0.0F, +1.0F, 0.0F, //
                -0.5F, +0.5F, -0.5F, 0.0F, +1.0F, 0.0F, //

                // -Z face
                -0.5F, -0.5F, -0.5F, 0.0F, 0.0F, -1.0F, //
                +0.5F, -0.5F, -0.5F, 0.0F, 0.0F, -1.0F, //
                +0.5F, +0.5F, -0.5F, 0.0F, 0.0F, -1.0F, //
                +0.5F, +0.5F, -0.5F, 0.0F, 0.0F, -1.0F, //
                -0.5F, +0.5F, -0.5F, 0.0F, 0.0F, -1.0F, //
                -0.5F, -0.5F, -0.5F, 0.0F, 0.0F, -1.0F, //

                // +Z face
                +0.5F, -0.5F, +0.5F, 0.0F, 0.0F, +1.0F, //
                -0.5F, -0.5F, +0.5F, 0.0F, 0.0F, +1.0F, //
                +0.5F, +0.5F, +0.5F, 0.0F, 0.0F, +1.0F, //
                -0.5F, +0.5F, +0.5F, 0.0F, 0.0F, +1.0F, //
                +0.5F, +0.5F, +0.5F, 0.0F, 0.0F, +1.0F, //
                -0.5F, -0.5F, +0.5F, 0.0F, 0.0F, +1.0F, //
            };
            auto vertexBuffer = renderDevice.createVertexBuffer(sizeof(buf), buf, Usage::Static);

            VertexArrayDesc desc{};
            desc.elementCount = 2;
            desc.elements[0].name = "position";
            desc.elements[0].type = Type::Float;
            desc.elements[0].size = 3;
            desc.elements[0].buffer.stride = 6 * sizeof(float);
            desc.elements[0].buffer.offset = 0;
            desc.elements[0].buffer.index = 0;
            desc.elements[1].name = "normal";
            desc.elements[1].type = Type::Float;
            desc.elements[1].size = 3;
            desc.elements[1].buffer.stride = 6 * sizeof(float);
            desc.elements[1].buffer.offset = 3 * sizeof(float);
            desc.elements[1].buffer.index = 0;
            desc.buffers[0] = cubos::core::memory::move(vertexBuffer);
            desc.shaderPipeline = pipeline;
            vertexArray = renderDevice.createVertexArray(desc);

            perSceneCB = renderDevice.createConstantBuffer(sizeof(PerScene), nullptr, Usage::Dynamic);
            perRopeCB = renderDevice.createConstantBuffer(sizeof(PerRope), nullptr, Usage::Dynamic);
        }
    };
} // namespace

void airships::client::ropePlugin(Cubos& cubos)
{
    static const Asset<Shader> VertexShader = AnyAsset("38cc5f9b-ca91-429d-84a5-1145e01b2731");
    static const Asset<Shader> PixelShader = AnyAsset("cdb7dbb1-401d-49e9-bca6-d33e28abdf9f");

    cubos.depends(transformPlugin);
    cubos.depends(assetsPlugin);
    cubos.depends(windowPlugin);
    cubos.depends(shaderPlugin);
    cubos.depends(gBufferPlugin);
    cubos.depends(cameraPlugin);
    cubos.depends(renderDepthPlugin);
    cubos.depends(hidePlugin);

    cubos.uninitResource<State>();

    cubos.relation<Rope>();

    cubos.component<RopeRasterizer>();

    cubos.startupSystem("setup Rope rasterizer")
        .tagged(assetsTag)
        .after(windowInitTag)
        .call([](Commands cmds, const Window& window, Assets& assets) {
            auto& rd = window->renderDevice();
            auto vs = assets.read(VertexShader)->shaderStage();
            auto ps = assets.read(PixelShader)->shaderStage();
            cmds.emplaceResource<State>(rd, rd.createShaderPipeline(vs, ps));
        });

    cubos.system("rasterize Rope relations")
        .tagged(drawToGBufferTag)
        .tagged(drawToRenderDepthTag)
        .with<LocalToWorld>()
        .without<Hide>()
        .related<Rope>()
        .with<LocalToWorld>()
        .without<Hide>()
        .call([](State& state, const Window& window, Query<const LocalToWorld&, const Rope&, const LocalToWorld&> ropes,
                 Query<const LocalToWorld&, Camera&, const DrawsTo&> cameras,
                 Query<Entity, RopeRasterizer&, GBuffer&, RenderDepth&> targets) {
            auto& rd = window->renderDevice();

            for (auto [ent, rasterizer, gBuffer, depth] : targets)
            {
                // Check if we need to recreate the framebuffer.
                if (rasterizer.position != gBuffer.position || rasterizer.normal != gBuffer.normal ||
                    rasterizer.albedo != gBuffer.albedo || rasterizer.depth != depth.texture)
                {
                    // Store textures so we can check if they change in the next frame.
                    rasterizer.position = gBuffer.position;
                    rasterizer.normal = gBuffer.normal;
                    rasterizer.albedo = gBuffer.albedo;
                    rasterizer.depth = depth.texture;

                    // Create the framebuffer.
                    FramebufferDesc desc{};
                    desc.targetCount = 3;
                    desc.depthStencil.setTexture2DTarget(depth.texture);
                    desc.targets[0].setTexture2DTarget(gBuffer.position);
                    desc.targets[1].setTexture2DTarget(gBuffer.normal);
                    desc.targets[2].setTexture2DTarget(gBuffer.albedo);
                    rasterizer.framebuffer = rd.createFramebuffer(desc);

                    CUBOS_INFO("Recreated RopeRasterizer's framebuffer");
                }

                // Bind the framebuffer and set the viewport.
                rd.setFramebuffer(rasterizer.framebuffer);
                rd.setViewport(0, 0, static_cast<int>(gBuffer.size.x), static_cast<int>(gBuffer.size.y));

                // Set the raster and depth-stencil states.
                rd.setRasterState(state.rasterState);
                rd.setBlendState(nullptr);
                rd.setDepthStencilState(state.depthStencilState);

                // Clear each target, as needed.
                if (!depth.cleared)
                {
                    rd.clearDepth(1.0F);
                    depth.cleared = true;
                }

                if (!gBuffer.cleared)
                {
                    rd.clearTargetColor(0, 0.0F, 0.0F, 0.0F, 0.0F);
                    rd.clearTargetColor(1, 0.0F, 0.0F, 0.0F, 0.0F);
                    rd.clearTargetColor(2, 0.0F, 0.0F, 0.0F, 0.0F);
                    gBuffer.cleared = true;
                }

                // Find the active cameras for this target.
                for (auto [cameraLocalToWorld, camera, drawsTo] : cameras.pin(1, ent))
                {
                    // Skip inactive cameras.
                    if (!camera.active)
                    {
                        continue;
                    }

                    // Send the PerScene data to the GPU.
                    auto view = glm::inverse(cameraLocalToWorld.mat);
                    auto proj = camera.projection;
                    PerScene perScene{.viewProj = proj * view};
                    state.perSceneCB->fill(&perScene, sizeof(perScene));

                    // Set the viewport.
                    rd.setViewport(static_cast<int>(drawsTo.viewportOffset.x * float(gBuffer.size.x)),
                                   static_cast<int>(drawsTo.viewportOffset.y * float(gBuffer.size.y)),
                                   static_cast<int>(drawsTo.viewportSize.x * float(gBuffer.size.x)),
                                   static_cast<int>(drawsTo.viewportSize.y * float(gBuffer.size.y)));
                    rd.setScissor(static_cast<int>(drawsTo.viewportOffset.x * float(gBuffer.size.x)),
                                  static_cast<int>(drawsTo.viewportOffset.y * float(gBuffer.size.y)),
                                  static_cast<int>(drawsTo.viewportSize.x * float(gBuffer.size.x)),
                                  static_cast<int>(drawsTo.viewportSize.y * float(gBuffer.size.y)));
                    // Bind the shader, vertex array and uniform buffer.
                    rd.setShaderPipeline(state.pipeline);
                    rd.setVertexArray(state.vertexArray);
                    state.perSceneBP->bind(state.perSceneCB);
                    state.perRopeBP->bind(state.perRopeCB);

                    // Iterate over all ropes and issue draw calls.
                    for (auto [fromLTW, rope, toLTW] : ropes)
                    {
                        auto fromPos = fromLTW.worldPosition();
                        auto toPos = toLTW.worldPosition();

                        // First, we scale the rope to the correct thickness and length.
                        auto mat = glm::scale(glm::mat4(1.0F),
                                              glm::vec3(rope.thickness, rope.thickness, glm::length(toPos - fromPos)));

                        // Then, we rotate it so that it points from the first entity to the second.
                        auto forward = glm::normalize(toPos - fromPos);
                        auto right = glm::normalize(glm::cross(forward, glm::vec3(0.0F, 1.0F, 0.0F)));
                        auto up = glm::cross(right, forward);
                        mat = glm::mat4(glm::vec4(right, 0.0F), glm::vec4(up, 0.0F), glm::vec4(forward, 0.0F),
                                        glm::vec4(0.0F, 0.0F, 0.0F, 1.0F)) *
                              mat;

                        // // Finally, we translate it to the midpoint between the two entities.
                        mat = glm::translate(glm::mat4(1.0F), (fromPos + toPos) * 0.5F) * mat;

                        // Send the PerRope data to the GPU and issue the draw call.
                        PerRope perRope{.model = mat, .color = glm::vec4(rope.color, 1.0F)};
                        state.perRopeCB->fill(&perRope, sizeof(perRope));
                        rd.drawTriangles(0, 36);
                    }
                }
            }
        });
}
