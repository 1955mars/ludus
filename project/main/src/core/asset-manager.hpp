#pragma once

#include "asset-inventory.hpp"
#include <vector>

namespace questart
{
    struct AssetManager
    {
        virtual void loadPipelines(const std::vector<questart::assets::Pipeline>& pipelines) = 0;

        virtual void loadStaticMeshes(const std::vector<questart::assets::StaticMesh>& staticMeshes) = 0;

        virtual void loadTextures(const std::vector<questart::assets::Texture>& textures) = 0;
    };
} // namespace questart