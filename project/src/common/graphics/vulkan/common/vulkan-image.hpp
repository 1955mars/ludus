#pragma once

#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/internal_ptr.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-command-pool.hpp"

namespace questart
{
    struct VulkanImage
    {
        VulkanImage(const questart::VulkanCommandPool& commandPool,
                    const questart::VulkanPhysicalDevice& physicalDevice,
                    const questart::VulkanDevice& device,
                    const uint32_t& width,
                    const uint32_t& height,
                    const uint32_t& depth,
                    const uint32_t& mipLevels,
                    const uint32_t& layerCount,
                    const uint32_t& faceCount,
                    const vk::SampleCountFlagBits& sampleCount,
                    const vk::Format& format,
                    const vk::ImageTiling& tiling,
                    const vk::ImageUsageFlags& usageFlags,
                    const vk::MemoryPropertyFlags& memoryFlags,
                    const vk::ImageLayout& oldLayout,
                    const vk::ImageLayout& newLayout);

        uint32_t getWidth() const;

        uint32_t getHeight() const;

        uint32_t getMipLevels() const;

        const vk::Format& getFormat() const;

        const vk::Image& getImage() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart