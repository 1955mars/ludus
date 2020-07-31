#include "vulkan-asset-manager.hpp"
#include "../../core/assets.hpp"

using questart::VulkanAssetManager;

struct VulkanAssetManager::Internal
{
    Internal() {}
};

VulkanAssetManager::VulkanAssetManager() : internal(questart::make_internal_ptr<Internal>()) {}

void VulkanAssetManager::loadPipelines(const std::vector<questart::assets::Pipeline>& pipelines)
{
    // TODO: Implement me
}

void VulkanAssetManager::loadStaticMeshes(const std::vector<questart::assets::StaticMesh>& staticMeshes)
{
    // TODO: Implement me
}

void VulkanAssetManager::loadTextures(const std::vector<questart::assets::Texture>& textures)
{
    // TODO: Implement me
}