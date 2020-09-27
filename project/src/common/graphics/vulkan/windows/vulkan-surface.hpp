#pragma once

#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/internal_ptr.hpp"
#include "../../../sdl/sdl-window.hpp"
#include "../common/vulkan-physical-device.hpp"

namespace questart
{
    struct VulkanSurface
    {
        VulkanSurface(const vk::Instance& instance,
                      const questart::VulkanPhysicalDevice& physicalDevice,
                      const questart::SDLWindow& window);

        const vk::SurfaceKHR& getSurface() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart