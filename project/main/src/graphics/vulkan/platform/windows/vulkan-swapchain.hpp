#pragma once

#include "../../../../core/graphics-wrapper.hpp"
#include "../../../../core/internal_ptr.hpp"
#include "../../vulkan-device.hpp"
#include "../../vulkan-physical-device.hpp"
#include "../../vulkan-image-view.hpp"
#include "vulkan-surface.hpp"

#include <vector>
#include <optional>

namespace questart
{
    struct VulkanSwapchain
    {
        VulkanSwapchain(const vk::Instance& instance,
                        const questart::VulkanPhysicalDevice& physicalDevice,
                        const questart::VulkanDevice& device,
                        const vk::SwapchainKHR& oldSwapchain);

        const vk::SwapchainKHR& getSwapchain() const;
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