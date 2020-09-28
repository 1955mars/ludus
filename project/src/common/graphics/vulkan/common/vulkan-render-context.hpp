#pragma once

#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/internal_ptr.hpp"
#include "../../../core/window-size.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-command-pool.hpp"



namespace questart
{
    struct VulkanRenderContext
    {
        VulkanRenderContext(const vk::Instance& instance,
                            const questart::VulkanPhysicalDevice& physicalDevice,
                            const questart::VulkanDevice& device,
                            const questart::VulkanCommandPool& commandPool,
                            const vk::SwapchainKHR& oldSwapchain = vk::SwapchainKHR());
/*
        questart::VulkanRenderContext recreate(const vk::Instance& instance,
                                    const questart::VulkanPhysicalDevice& physicalDevice,
                                    const questart::VulkanDevice& device,
                                    const questart::VulkanCommandPool& commandPool);
                                    */

        bool renderBegin(const questart::VulkanDevice& device);
        bool renderEnd(const questart::VulkanDevice& device, const double& displayTime, void* tracking, long long frameIndex);

        
        const vk::Viewport& getViewport() const;

        const vk::Rect2D& getScissor() const;

        const vk::RenderPass& getRenderPass() const;

        const vk::CommandBuffer& getActiveCommandBuffer() const;

        const questart::WindowSize& getCurrentWindowSize();

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart