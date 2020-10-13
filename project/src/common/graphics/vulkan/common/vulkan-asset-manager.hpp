#pragma once

#include "../../../core/asset-manifest.hpp"
#include "../../../core/internal_ptr.hpp"
#include "vulkan-command-pool.hpp"
#include "vulkan-device.hpp"
#include "vulkan-model.hpp"
#include "vulkan-mesh.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-render-context.hpp"
#include "vulkan-texture.hpp"
#include "vulkan-pipeline.hpp"

namespace questart
{
    struct VulkanAssetManager
    {
        VulkanAssetManager();

        void loadAssetManifest(const questart::VulkanPhysicalDevice& physicalDevice,
                               const questart::VulkanDevice& device,
                               const questart::VulkanRenderContext& renderContext,
                               const questart::VulkanCommandPool& commandPool,
                               const questart::AssetManifest& assetManifest);

        const questart::VulkanMesh& getStaticMesh(const questart::assets::StaticMesh& staticMesh) const;

        void reloadContextualAssets(const questart::VulkanPhysicalDevice& physicalDevice,
                                    const questart::VulkanDevice& device,
                                    const questart::VulkanRenderContext& renderContext);

        const questart::VulkanTexture& getTexture(const questart::assets::Texture& texture) const;

        const questart::VulkanPipeline& getPipeline(const questart::assets::Pipeline& pipeline) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart