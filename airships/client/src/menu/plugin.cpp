#include "plugin.hpp"
#include <cubos/engine/assets/plugin.hpp>
#include <cubos/engine/render/target/target.hpp>
#include <cubos/engine/scene/scene.hpp>
#include <cubos/engine/transform/child_of.hpp>
#include <cubos/engine/ui/canvas/canvas.hpp>
#include <cubos/engine/ui/image/image.hpp>
#include <cubos/engine/ui/canvas/plugin.hpp>
#include <cubos/engine/ui/image/plugin.hpp>
#include <cubos/engine/ui/text/plugin.hpp>
#include <cubos/engine/ui/canvas/element.hpp>
#include <cubos/engine/render/target/plugin.hpp>
#include <cubos/engine/window/plugin.hpp>
#include <cubos/engine/settings/plugin.hpp>

using namespace cubos::engine;

// static const Asset<Scene> MenuSceneAsset = AnyAsset("21e4d65f-ca53-4edc-9144-a3df8a131967");
// static const Asset<Image> BackgroundAsset = AnyAsset("1b6dd65e-7b2c-40c6-8612-af22e6e0a9d1");
// static const Asset<Image> TitleAsset = AnyAsset("a04bb3d8-7eac-41fe-b69d-cf167dd7fd90");

void airships::client::menuPlugin(Cubos& cubos)
{
    cubos.depends(assetsPlugin);
    cubos.depends(uiCanvasPlugin);
    cubos.depends(uiImagePlugin);
    cubos.depends(uiTextPlugin);
    cubos.depends(renderTargetPlugin);
    cubos.depends(settingsPlugin);
    cubos.depends(windowPlugin);

    cubos.startupSystem("load ui").after(windowInitTag).after(settingsTag).call([](Commands cmds) {
        auto canvas = cmds.create().add(UICanvas{}).add(RenderTarget{}).entity();
        auto background = cmds.create()
                              .add(UIElement{})
                              .add(UIImage{.asset = AnyAsset("1b6dd65e-7b2c-40c6-8612-af22e6e0a9d1")})
                              .entity();
        cmds.relate(background, canvas, ChildOf{});
    });

    // cubos.startupSystem("load and spawn main menu").tagged(assetsTag).call([](Commands cmds, Assets& assets) {
    //     auto bp = assets.read(MenuSceneAsset)->blueprint;
    //     for (auto& ent : bp.entities())
    //     {
    //         if (ent.second == "background")
    //         {
    //             bp.add(ent.first, UIImage{.asset = BackgroundAsset});
    //         }
    //     }
    //     cmds.spawn(bp);
    // });
}
