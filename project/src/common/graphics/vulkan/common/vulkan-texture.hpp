#pragma once

#include "../../../core/asset-inventory.hpp"
#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/internal_ptr.hpp"
#include "vulkan-command-pool.hpp"
#include "vulkan-device.hpp"
#include "vulkan-image-view.hpp"
#include "vulkan-physical-device.hpp"

namespace questart
{
    struct VulkanTexture
    {
        VulkanTexture(const questart::assets::Texture& textureId,
                      const questart::VulkanPhysicalDevice& physicalDevice,
                      const questart::VulkanDevice& device,
                      const questart::VulkanCommandPool& commandPool,
                      const std::string& texturePath);

        const questart::assets::Texture& getTextureId() const;

        const questart::VulkanImageView& getImageView() const;

        const vk::Sampler& getSampler() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart