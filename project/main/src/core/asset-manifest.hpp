#pragma once

#include "asset-inventory.hpp"
#include <vector>

namespace questart
{
    struct AssetManifest
    {
        const std::vector<questart::assets::Pipeline> pipelines;

        const std::vector<questart::assets::StaticMesh> staticMeshes;

        const std::vector<questart::assets::Texture> textures;
    };
} // namespace questart