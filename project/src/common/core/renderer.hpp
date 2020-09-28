#pragma once

#include "asset-manifest.hpp"
#include "asset-inventory.hpp"
#include "static-mesh-instance.hpp"
#include "window-size.hpp"
#include <vector>

namespace questart
{
    struct Renderer
    {
        virtual void loadAssetManifest(const questart::AssetManifest& assetManifest) = 0;

        virtual bool renderBegin() = 0;

        virtual void render(
            const questart::assets::Pipeline& pipeline,
            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) = 0;

        virtual void renderEnd(const double& displayTime, void* tracking, long long frameIndex) = 0;

        virtual const questart::WindowSize& getCurrentWindowSize() = 0;
    };
} // namespace ast