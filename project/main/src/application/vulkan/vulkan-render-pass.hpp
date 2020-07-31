#pragma once

#include "../../core/internal_ptr.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-swapchain.hpp"

namespace questart
{
    struct VulkanRenderPass
    {
        VulkanRenderPass(const questart::VulkanPhysicalDevice& physicalDevice,
                         const questart::VulkanDevice& device,
                         const questart::VulkanSwapchain& swapchain);

        const vk::RenderPass& getRenderPass() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart