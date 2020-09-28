#pragma once

#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/internal_ptr.hpp"
#include "../common/vulkan-device.hpp"
#include "../common/vulkan-physical-device.hpp"
#include "../common/vulkan-image-view.hpp"
#include "../../../core/window-size.hpp"
#include "vulkan-command-pool.hpp"
#include "ovr-wrapper.hpp"
#include <vector>
#include <optional>

namespace questart
{
    struct VulkanSwapchain
    {
        VulkanSwapchain(const vk::Instance& instance,
                        const questart::VulkanPhysicalDevice& physicalDevice,
                        const questart::VulkanDevice& device,
                        const questart::VulkanCommandPool& commandPool,
                        const vk::SwapchainKHR& oldSwapchain);

        ovrTextureSwapChain* getSwapChain() const;
        const std::vector<questart::VulkanImageView>& getImageViews() const;
        const vk::Format& getColorFormat() const;
        const vk::Extent2D& getExtent() const;
        uint32_t getImageCount() const;

        const questart::WindowSize& getCurrentWindowSize() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart