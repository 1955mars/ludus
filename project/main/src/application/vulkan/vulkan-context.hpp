#pragma once

#include "../../core/internal_ptr.hpp"
#include "../../core/renderer.hpp"
#include "vulkan-asset-manager.hpp"

namespace questart
{
    struct VulkanContext : public questart::Renderer
    {
        VulkanContext(std::shared_ptr<questart::VulkanAssetManager> assetManager);

        bool renderBegin();

        void render(
            const questart::assets::Pipeline& pipeline,
            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) override;

        void renderEnd();

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart