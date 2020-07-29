#pragma once

#include "../../core/internal_ptr.hpp"
#include "../../core/renderer.hpp"
#include "opengl-asset-manager.hpp"
#include <memory>

namespace questart
{
    struct OpenGLRenderer : public questart::Renderer
    {
        OpenGLRenderer(std::shared_ptr<questart::OpenGLAssetManager> assetManager);

        void render(
            const questart::assets::Pipeline& pipeline,
            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart