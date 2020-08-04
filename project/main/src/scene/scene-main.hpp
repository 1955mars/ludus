#pragma once

#include "../core/internal_ptr.hpp"
#include "scene.hpp"

namespace questart
{
    struct SceneMain : public questart::Scene
    {
        SceneMain(const float& screenWidth, const float& screenHeight);

        questart::AssetManifest getAssetManifest() override;

        void prepare() override;

        void update(const float& delta) override;

        void render(questart::Renderer& renderer) override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart