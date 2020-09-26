#include "vulkan-image-view.hpp"

using questart::VulkanImageView;

namespace
{
    vk::UniqueImageView createImageView(const vk::Device& device,
                                        const vk::Image& image,
                                        const vk::Format& format,
                                        const vk::ImageAspectFlags& aspectFlags,
                                        const uint32_t& mipLevels,
                                        const uint32_t& layerCount,
                                        const uint32_t& faceCount)
    {
        vk::ImageSubresourceRange subresourceRangeInfo{
            aspectFlags, // Flags
            0,           // Base mip level
            mipLevels,   // Mip level count
            0,           // Base array layer
            faceCount * layerCount            // Layer count
        };

        vk::ImageViewCreateInfo createInfo{
            vk::ImageViewCreateFlags(), // Flags
            image,                      // Image
            layerCount == 2 ? vk::ImageViewType::e2DArray : vk::ImageViewType::e2D, // View type
            format,                     // Format
            vk::ComponentMapping(),     // Components
            subresourceRangeInfo        // Subresource range
        };

        return device.createImageViewUnique(createInfo);
    }
} // namespace

struct VulkanImageView::Internal
{
    const vk::UniqueImageView imageView;

    Internal(const vk::Device& device,
             const vk::Image& image,
             const vk::Format& format,
             const vk::ImageAspectFlags& aspectFlags,
             const uint32_t& mipLevels,
             const uint32_t& layerCount,
             const uint32_t& faceCount)
        : imageView(::createImageView(device, image, format, aspectFlags, mipLevels, layerCount, faceCount)) {}
};

VulkanImageView::VulkanImageView(const vk::Device& device,
                                 const vk::Image& image,
                                 const vk::Format& format,
                                 const vk::ImageAspectFlags& aspectFlags,
                                 const uint32_t& mipLevels,
                                 const uint32_t& layerCount,
                                 const uint32_t& faceCount)
    : internal(questart::make_internal_ptr<Internal>(device, image, format, aspectFlags, mipLevels, layerCount, faceCount)) {}

const vk::ImageView& VulkanImageView::getImageView() const
{
    return internal->imageView.get();
}