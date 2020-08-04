#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"

namespace questart
{
    struct VulkanPhysicalDevice
    {
        VulkanPhysicalDevice(const vk::Instance& instance);

        const vk::PhysicalDevice& getPhysicalDevice() const;
        vk::SampleCountFlagBits getMultiSamplingLevel() const;
        vk::Format getDepthFormat() const;
        uint32_t getMemoryTypeIndex(const uint32_t& filter, const vk::MemoryPropertyFlags& flags) const;
        bool isShaderMultiSamplingSupported() const;
        bool isAnisotropicFilteringSupported() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart