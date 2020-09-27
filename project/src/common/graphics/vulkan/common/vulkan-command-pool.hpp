#pragma once

#include "../../../core/internal_ptr.hpp"
#include "vulkan-device.hpp"
#include <vector>

namespace questart
{
    struct VulkanCommandPool
    {
        VulkanCommandPool(const questart::VulkanDevice& device);

        vk::UniqueCommandBuffer beginCommandBuffer(const questart::VulkanDevice& device) const;

        void endCommandBuffer(const vk::CommandBuffer& commandBuffer,
                              const questart::VulkanDevice& device) const;

        std::vector<vk::UniqueCommandBuffer> createCommandBuffers(const questart::VulkanDevice& device,
                                                                  const uint32_t& count) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart