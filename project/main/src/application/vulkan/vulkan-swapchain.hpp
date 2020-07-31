#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "../../core/sdl-window.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-surface.hpp"
#include "vulkan-image-view.hpp"

#include <vector>

namespace questart
{
    struct VulkanSwapchain
    {
        VulkanSwapchain(const questart::SDLWindow& window,
                        const questart::VulkanPhysicalDevice& physicalDevice,
                        const questart::VulkanDevice& device,
                        const questart::VulkanSurface& surface);

        const vk::SwapchainKHR& getSwapchain() const;
        const std::vector<questart::VulkanImageView>& getImageViews() const;
        const vk::Format& getColorFormat() const;
        const vk::Extent2D& getExtent() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart