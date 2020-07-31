#pragma once

#include "../../core/internal_ptr.hpp"
#include "vulkan-device.hpp"

namespace questart
{
    struct VulkanCommandPool
    {
        VulkanCommandPool(const questart::VulkanDevice& device);

        vk::UniqueCommandBuffer beginCommandBuffer(const questart::VulkanDevice& device) const;

        void endCommandBuffer(const vk::CommandBuffer& commandBuffer,
                              const questart::VulkanDevice& device) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart