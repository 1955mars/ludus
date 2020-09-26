#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"
#include "../../core/mesh.hpp"
#include "vulkan-command-pool.hpp"
#include "vulkan-device.hpp"
#include "vulkan-physical-device.hpp"

namespace questart
{
    struct VulkanMesh
    {
        VulkanMesh(const questart::VulkanPhysicalDevice& physicalDevice,
                   const questart::VulkanDevice& device,
                   const questart::VulkanCommandPool& commandPool,
                   const questart::Mesh& mesh);

        const vk::Buffer& getVertexBuffer() const;

        const vk::Buffer& getIndexBuffer() const;

        const uint32_t& getNumIndices() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart