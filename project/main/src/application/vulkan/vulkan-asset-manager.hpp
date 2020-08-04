#pragma once

#include "../../core/asset-manifest.hpp"
#include "../../core/internal_ptr.hpp"
#include "vulkan-render-context.hpp"

namespace questart
{
    struct VulkanAssetManager
    {
        VulkanAssetManager();

        void loadAssetManifest(const questart::VulkanPhysicalDevice& physicalDevice,
                               const questart::VulkanDevice& device,
                               const questart::VulkanRenderContext& renderContext,
                               const questart::AssetManifest& assetManifest);

        void reloadContextualAssets(const questart::VulkanPhysicalDevice& physicalDevice,
                                    const questart::VulkanDevice& device,
                                    const questart::VulkanRenderContext& renderContext);

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart