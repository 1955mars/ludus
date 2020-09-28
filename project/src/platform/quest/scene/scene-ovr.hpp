#pragma once

#include "internal_ptr.hpp"
#include "glm-wrapper.hpp"
#include "ovr-wrapper.hpp"
#include "scene.hpp"

namespace questart
{
    struct SceneOVR : public questart::Scene
    {
        SceneOVR();

        questart::AssetManifest getAssetManifest() override;

        void prepare() override;

        void update(const float& delta, void* tracking) override ;

        void render(questart::Renderer& renderer) override;

        void onWindowResized(const questart::WindowSize& size) override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
}