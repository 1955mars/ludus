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
    const questart::VulkanDevice& device;
    const vk::UniqueBuffer buffer;
    const vk::UniqueDeviceMemory deviceMemory;
    void* mappedMemory;

    Internal(const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const vk::DeviceSize& size,
             const vk::BufferUsageFlags& bufferFlags,
             const vk::MemoryPropertyFlags& memoryFlags,
             const void* dataSource, bool mapMemory)
        : device(device),
          buffer(::createBuffer(device, size, bufferFlags)),
          deviceMemory(::allocateMemory(physicalDevice, device, buffer.get(), memoryFlags)),
          mappedMemory(nullptr)
    {
        // Take the buffer and the allocated memory and bind them together.
        device.getDevice().bindBufferMemory(buffer.get(), deviceMemory.get(), 0);

        // Take the datasource and copy it into our allocated memory block.
        if (dataSource)
        {
            mappedMemory = device.getDevice().mapMemory(deviceMemory.get(), 0, size);
            std::memcpy(mappedMemory, dataSource, static_cast<size_t>(size));
            if (!mapMemory)
            {
                device.getDevice().unmapMemory(deviceMemory.get());
                mappedMemory = nullptr;
            }
        }
    }

    void copyData(const vk::DeviceSize& size, const void* dataSource)
    {
        if (dataSource)
        {
            if (!mappedMemory)
            {
                mappedMemory = device.getDevice().mapMemory(deviceMemory.get(), 0, size);
            }
            std::memcpy(mappedMemory, dataSource, static_cast<size_t>(size));
        }
    }
};

VulkanBuffer::VulkanBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                           const questart::VulkanDevice& device,
                           const vk::DeviceSize& size,
                           const vk::BufferUsageFlags& bufferFlags,
                           const vk::MemoryPropertyFlags& memoryFlags,
                           const void* dataSource, bool mapMemory)
    : internal(questart::make_internal_ptr<Internal>(physicalDevice, device, size, bufferFlags, memoryFlags, dataSource, mapMemory)) {}

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

void VulkanBuffer::copyData(const vk::DeviceSize& size, const void* dataSource)
{
    internal->copyData(size, dataSource);
}
