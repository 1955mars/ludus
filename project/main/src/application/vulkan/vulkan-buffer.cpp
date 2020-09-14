#include "vulkan-buffer.hpp"

using questart::VulkanBuffer;

namespace
{
    vk::UniqueBuffer createBuffer(const questart::VulkanDevice& device,
                                  const vk::DeviceSize& size,
                                  const vk::BufferUsageFlags& bufferFlags)
    {
        vk::BufferCreateInfo info{
            vk::BufferCreateFlags(),     // Flags
            size,                        // Size
            bufferFlags,                 // Buffer usage flags
            vk::SharingMode::eExclusive, // Sharing mode
            0,                           // Queue family index count
            nullptr};                    // Queue family indices

        return device.getDevice().createBufferUnique(info);
    }

    vk::UniqueDeviceMemory allocateMemory(const questart::VulkanPhysicalDevice& physicalDevice,
                                          const questart::VulkanDevice& device,
                                          const vk::Buffer& buffer,
                                          const vk::MemoryPropertyFlags& memoryFlags)
    {
        vk::MemoryRequirements memoryRequirements{
            device.getDevice().getBufferMemoryRequirements(buffer)};

        uint32_t memoryTypeIndex{
            physicalDevice.getMemoryTypeIndex(memoryRequirements.memoryTypeBits, memoryFlags)};

        vk::MemoryAllocateInfo info{
            memoryRequirements.size, // Allocation size
            memoryTypeIndex};        // Memory type index

        return device.getDevice().allocateMemoryUnique(info);
    }
} // namespace

struct VulkanBuffer::Internal
{
    const vk::UniqueBuffer buffer;
    const vk::UniqueDeviceMemory deviceMemory;

    Internal(const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const vk::DeviceSize& size,
             const vk::BufferUsageFlags& bufferFlags,
             const vk::MemoryPropertyFlags& memoryFlags,
             const void* dataSource)
        : buffer(::createBuffer(device, size, bufferFlags)),
          deviceMemory(::allocateMemory(physicalDevice, device, buffer.get(), memoryFlags))
    {
        // Take the buffer and the allocated memory and bind them together.
        device.getDevice().bindBufferMemory(buffer.get(), deviceMemory.get(), 0);

        // Take the datasource and copy it into our allocated memory block.
        if (dataSource)
        {
            void* mappedMemory{device.getDevice().mapMemory(deviceMemory.get(), 0, size)};
            std::memcpy(mappedMemory, dataSource, static_cast<size_t>(size));
            device.getDevice().unmapMemory(deviceMemory.get());
        }
    }

    void copyData(const questart::VulkanDevice& device, const vk::DeviceSize& size, const void* dataSource)
    {
        if (dataSource)
        {
            void* mappedMemory{device.getDevice().mapMemory(deviceMemory.get(), 0, size)};
            std::memcpy(mappedMemory, dataSource, static_cast<size_t>(size));
            device.getDevice().unmapMemory(deviceMemory.get());
        }
    }
};

VulkanBuffer::VulkanBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                           const questart::VulkanDevice& device,
                           const vk::DeviceSize& size,
                           const vk::BufferUsageFlags& bufferFlags,
                           const vk::MemoryPropertyFlags& memoryFlags,
                           const void* dataSource)
    : internal(questart::make_internal_ptr<Internal>(physicalDevice, device, size, bufferFlags, memoryFlags, dataSource)) {}

const vk::Buffer& VulkanBuffer::getBuffer() const
{
    return internal->buffer.get();
}

questart::VulkanBuffer VulkanBuffer::createDeviceLocalBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                                                        const questart::VulkanDevice& device,
                                                        const questart::VulkanCommandPool& commandPool,
                                                        const vk::DeviceSize& size,
                                                        const vk::BufferUsageFlags& bufferFlags,
                                                        const void* dataSource)
{
    questart::VulkanBuffer stagingBuffer{
        physicalDevice,
        device,
        size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        dataSource};

    questart::VulkanBuffer deviceLocalBuffer{
        physicalDevice,
        device,
        size,
        vk::BufferUsageFlagBits::eTransferDst | bufferFlags,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        nullptr};

    vk::UniqueCommandBuffer commandBuffer{commandPool.beginCommandBuffer(device)};

    vk::BufferCopy copyRegion{
        0,     // Source offset
        0,     // Destination offset
        size}; // Size

    commandBuffer->copyBuffer(stagingBuffer.getBuffer(), deviceLocalBuffer.getBuffer(), 1, &copyRegion);

    commandPool.endCommandBuffer(commandBuffer.get(), device);

    return deviceLocalBuffer;
}

void VulkanBuffer::copyData(const questart::VulkanDevice& device, const vk::DeviceSize& size, const void* dataSource)
{
    internal->copyData(device, size, dataSource);
}
