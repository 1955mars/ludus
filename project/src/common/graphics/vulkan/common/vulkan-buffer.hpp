#pragma once

#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/internal_ptr.hpp"
#include "vulkan-command-pool.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"

namespace questart
{
    struct VulkanBuffer
    {
        VulkanBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                     const questart::VulkanDevice& device,
                     const vk::DeviceSize& size,
                     const vk::BufferUsageFlags& bufferFlags,
                     const vk::MemoryPropertyFlags& memoryFlags,
                     const void* dataSource);

        const vk::Buffer& getBuffer() const;

        static questart::VulkanBuffer createDeviceLocalBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                                                         const questart::VulkanDevice& device,
                                                         const questart::VulkanCommandPool& commandPool,
                                                         const vk::DeviceSize& size,
                                                         const vk::BufferUsageFlags& bufferFlags,
                                                         const void* dataSource);

        void copyData(const questart::VulkanDevice& device, const vk::DeviceSize& size, const void* dataSource);

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart