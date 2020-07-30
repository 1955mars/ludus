#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "../../core/sdl-window.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-surface.hpp"

namespace questart
{
    struct VulkanRenderContext
    {
        VulkanRenderContext(const questart::SDLWindow& window,
                            const questart::VulkanPhysicalDevice& physicalDevice,
                            const questart::VulkanDevice& device,
                            const questart::VulkanSurface& surface);

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart