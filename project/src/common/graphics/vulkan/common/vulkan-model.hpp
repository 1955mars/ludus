#pragma once

#include "internal_ptr.hpp"
#include "glm-wrapper.hpp"
#include "vulkan-device.hpp"
#include "vulkan-image.hpp"
#include "vulkan-texture.hpp"

#if defined(QUEST)
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif

#define MAX_NUM_JOINTS 128u

namespace questart
{
    struct VulkanModel
    {
        VulkanModel(const questart::VulkanPhysicalDevice& physicalDevice,
                    const questart::VulkanDevice& device,
                    const questart::VulkanCommandPool& commandPool,
                    const std::string& path);

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
}
