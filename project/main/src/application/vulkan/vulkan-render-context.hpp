#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "../../core/sdl-window.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-surface.hpp"
#include "vulkan-command-pool.hpp"

namespace questart
{
    struct VulkanRenderContext
    {
        VulkanRenderContext(const questart::SDLWindow& window,
                            const questart::VulkanPhysicalDevice& physicalDevice,
                            const questart::VulkanDevice& device,
                            const questart::VulkanSurface& surface,
                            const questart::VulkanCommandPool& commandPool,
                            const vk::SwapchainKHR& oldSwapchain = vk::SwapchainKHR());

        questart::VulkanRenderContext recreate(const questart::SDLWindow& window,
                                    const questart::VulkanPhysicalDevice& physicalDevice,
                                    const questart::VulkanDevice& device,
                                    const questart::VulkanSurface& surface,
                                    const questart::VulkanCommandPool& commandPool);

        bool renderBegin(const questart::VulkanDevice& device);
        bool renderEnd(const questart::VulkanDevice& device);

        
        const vk::Viewport& getViewport() const;

        const vk::Rect2D& getScissor() const;

        const vk::RenderPass& getRenderPass() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart