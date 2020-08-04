#include "vulkan-mesh.hpp"
#include "vulkan-buffer.hpp"
#include <vector>

using questart::VulkanMesh;

namespace
{
    questart::VulkanBuffer createVertexBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                                         const questart::VulkanDevice& device,
                                         const questart::VulkanCommandPool& commandPool,
                                         const questart::Mesh& mesh)
    {
        return questart::VulkanBuffer::createDeviceLocalBuffer(physicalDevice,
                                                          device,
                                                          commandPool,
                                                          sizeof(questart::Vertex) * mesh.getNumVertices(),
                                                          vk::BufferUsageFlagBits::eVertexBuffer,
                                                          mesh.getVertices().data());
    }

    questart::VulkanBuffer createIndexBuffer(const questart::VulkanPhysicalDevice& physicalDevice,
                                        const questart::VulkanDevice& device,
                                        const questart::VulkanCommandPool& commandPool,
                                        const questart::Mesh& mesh)
    {
        return questart::VulkanBuffer::createDeviceLocalBuffer(physicalDevice,
                                                          device,
                                                          commandPool,
                                                          sizeof(uint32_t) * mesh.getNumIndices(),
                                                          vk::BufferUsageFlagBits::eIndexBuffer,
                                                          mesh.getIndices().data());
    }
} // namespace

struct VulkanMesh::Internal
{
    const questart::VulkanBuffer vertexBuffer;
    const questart::VulkanBuffer indexBuffer;
    const uint32_t numIndices;

    Internal(const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanCommandPool& commandPool,
             const questart::Mesh& mesh)
        : vertexBuffer(::createVertexBuffer(physicalDevice, device, commandPool, mesh)),
          indexBuffer(::createIndexBuffer(physicalDevice, device, commandPool, mesh)),
          numIndices(mesh.getNumIndices()) {}
};

VulkanMesh::VulkanMesh(const questart::VulkanPhysicalDevice& physicalDevice,
                       const questart::VulkanDevice& device,
                       const questart::VulkanCommandPool& commandPool,
                       const questart::Mesh& mesh)
    : internal(questart::make_internal_ptr<Internal>(physicalDevice, device, commandPool, mesh)) {}

const vk::Buffer& VulkanMesh::getVertexBuffer() const
{
    return internal->vertexBuffer.getBuffer();
}

const vk::Buffer& VulkanMesh::getIndexBuffer() const
{
    return internal->indexBuffer.getBuffer();
}

const uint32_t& VulkanMesh::getNumIndices() const
{
    return internal->numIndices;
}
