#pragma once

#include "../../core/asset-manager.hpp"
#include "../../core/internal_ptr.hpp"

namespace questart
{
    struct VulkanAssetManager : public questart::AssetManager
    {
        VulkanAssetManager();

        void loadPipelines(const std::vector<questart::assets::Pipeline>& pipelines) override;

        void loadStaticMeshes(const std::vector<questart::assets::StaticMesh>& staticMeshes) override;

        void loadTextures(const std::vector<questart::assets::Texture>& textures) override;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart