#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-surface.hpp"
#include <vector>

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

        const vk::Queue& getGraphicsQueue() const;
        const vk::Queue& getPresentationQueue() const;

        std::vector<vk::UniqueSemaphore> createSemaphores(const uint32_t& count) const;
        std::vector<vk::UniqueFence> createFences(const uint32_t& count) const;

        vk::UniqueShaderModule createShaderModule(const std::vector<char>& shaderCode) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart