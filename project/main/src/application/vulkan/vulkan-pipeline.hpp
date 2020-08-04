#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "../../core/static-mesh-instance.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include <string>
#include <vector>

namespace questart
{
    struct VulkanAssetManager;

    struct VulkanPipeline
    {
        VulkanPipeline(const questart::VulkanPhysicalDevice& physicalDevice,
                       const questart::VulkanDevice& device,
                       const std::string& shaderName,
                       const vk::Viewport& viewport,
                       const vk::Rect2D& scissor,
                       const vk::RenderPass& renderPass);

        void render(
            const questart::VulkanAssetManager& assetManager,
            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart