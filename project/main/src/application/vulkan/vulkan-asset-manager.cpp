#include "vulkan-asset-manager.hpp"
#include "../../core/assets.hpp"
#include "../../core/log.hpp"
#include "vulkan-pipeline.hpp"
#include <unordered_map>

using questart::VulkanAssetManager;

namespace
{
    questart::VulkanPipeline createPipeline(const questart::assets::Pipeline& pipeline,
                                       const questart::VulkanPhysicalDevice& physicalDevice,
                                       const questart::VulkanDevice& device,
                                       const questart::VulkanRenderContext& renderContext)
    {
        const std::string pipelinePath{questart::assets::resolvePipelinePath(pipeline)};

        questart::log("questart::VulkanAssetManager::createPipeline", "Creating pipeline: " + pipelinePath);

        return questart::VulkanPipeline(physicalDevice,
                                   device,
                                   pipelinePath,
                                   renderContext.getViewport(),
                                   renderContext.getScissor(),
                                   renderContext.getRenderPass());
    }

    questart::VulkanMesh createMesh(const questart::VulkanPhysicalDevice& physicalDevice,
                               const questart::VulkanDevice& device,
                               const questart::VulkanCommandPool& commandPool,
                               const questart::assets::StaticMesh& staticMesh)
    {
        std::string meshPath{questart::assets::resolveStaticMeshPath(staticMesh)};

        questart::log("questart::VulkanAssetManager::createMesh", "Creating static mesh from " + meshPath);

        return questart::VulkanMesh(physicalDevice,
                               device,
                               commandPool,
                               questart::assets::loadOBJFile(meshPath));
    }
} // namespace


struct VulkanAssetManager::Internal
{
    std::unordered_map<questart::assets::Pipeline, questart::VulkanPipeline> pipelineCache;
    std::unordered_map<questart::assets::StaticMesh, questart::VulkanMesh> staticMeshCache;

    Internal() {}

    void loadAssetManifest(const questart::VulkanPhysicalDevice& physicalDevice,
                           const questart::VulkanDevice& device,
                           const questart::VulkanRenderContext& renderContext,
                           const questart::VulkanCommandPool& commandPool,
                           const questart::AssetManifest& assetManifest)
    {
        for (const auto& pipeline : assetManifest.pipelines)
        {
            if (pipelineCache.count(pipeline) == 0)
            {
                pipelineCache.insert(std::make_pair(
                    pipeline,
                    ::createPipeline(pipeline, physicalDevice, device, renderContext)));
            }
        }

        for (const auto& staticMesh : assetManifest.staticMeshes)
        {
            if (staticMeshCache.count(staticMesh) == 0)
            {
                staticMeshCache.insert(std::make_pair(
                    staticMesh,
                    ::createMesh(physicalDevice, device, commandPool, staticMesh)));
            }
        }
    }

    void reloadContextualAssets(const questart::VulkanPhysicalDevice& physicalDevice,
                                const questart::VulkanDevice& device,
                                const questart::VulkanRenderContext& renderContext)
    {
        for (auto& element : pipelineCache)
        {
            element.second = ::createPipeline(element.first, physicalDevice, device, renderContext);
        }
    }
};

VulkanAssetManager::VulkanAssetManager() : internal(questart::make_internal_ptr<Internal>()) {}

void VulkanAssetManager::loadAssetManifest(const questart::VulkanPhysicalDevice& physicalDevice,
                                           const questart::VulkanDevice& device,
                                           const questart::VulkanRenderContext& renderContext,
                                           const questart::VulkanCommandPool& commandPool,
                                           const questart::AssetManifest& assetManifest)
{
    internal->loadAssetManifest(physicalDevice, device, renderContext, commandPool, assetManifest);
}

void VulkanAssetManager::reloadContextualAssets(const questart::VulkanPhysicalDevice& physicalDevice,
                                                const questart::VulkanDevice& device,
                                                const questart::VulkanRenderContext& renderContext)
{
    internal->reloadContextualAssets(physicalDevice, device, renderContext);
}

const questart::VulkanMesh& VulkanAssetManager::getStaticMesh(const questart::assets::StaticMesh& staticMesh) const
{
    return internal->staticMeshCache.at(staticMesh);
}