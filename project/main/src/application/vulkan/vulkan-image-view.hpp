#pragma once

#include "../../core/graphics-wrapper.hpp"
#include "../../core/internal_ptr.hpp"

namespace questart
{
    struct VulkanImageView
    {
        VulkanImageView(const vk::Device& device,
                        const vk::Image& image,
                        const vk::Format& format,
                        const vk::ImageAspectFlags& aspectFlags,
                        const uint32_t& mipLevels);

        const vk::ImageView& getImageView() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart