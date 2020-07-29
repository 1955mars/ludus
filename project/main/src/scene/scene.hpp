#pragma once

#include "../core/asset-manager.hpp"
#include "../core/renderer.hpp"

namespace questart
{
    struct Scene
    {
        Scene() = default;

        virtual ~Scene() = default;

        virtual void prepare(questart::AssetManager& assetManager) = 0;

        virtual void update(const float& delta) = 0;

        virtual void render(questart::Renderer& renderer) = 0;
    };
} // namespace questart