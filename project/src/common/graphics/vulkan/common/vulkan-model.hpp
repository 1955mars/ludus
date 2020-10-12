#pragma once

#include "internal_ptr.hpp"
#include "glm-wrapper.hpp"
#include "vulkan-device.hpp"
#include "vulkan-image.hpp"
#include "vulkan-texture.hpp"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#if defined(QUEST)
#define TINYGLTF_ANDROID_LOAD_FROM_ASSETS
#endif

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
