#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-surface.hpp"

namespace questart
{
    struct VulkanDevice
    {
        VulkanDevice(const questart::VulkanPhysicalDevice& physicalDevice,
                     const questart::VulkanSurface& surface);

        const vk::Device& getDevice() const;

        uint32_t getGraphicsQueueIndex() const;

        uint32_t getPresentationQueueIndex() const;

        bool hasDiscretePresentationQueue() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart