#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"

namespace questart
{
    struct VulkanPhysicalDevice
    {
        VulkanPhysicalDevice(const vk::Instance& instance);

        const vk::PhysicalDevice& getPhysicalDevice() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart