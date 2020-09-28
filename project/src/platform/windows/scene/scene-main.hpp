#pragma once

#include "ludus.hpp"
#include "scene.hpp"

namespace questart
{
    struct SceneMain : public questart::Scene
    {
        SceneMain(const questart::WindowSize& frameSize);

        questart::AssetManifest getAssetManifest() override;

        void prepare() override;

        void update(const float& delta, void* tracking) override;

        void render(questart::Renderer& renderer) override;

        void onWindowResized(const questart::WindowSize& size) override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart