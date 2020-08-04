#include "vulkan-texture.hpp"
#include "vulkan-buffer.hpp"
#include "vulkan-image.hpp"
#include <cmath>

using questart::VulkanTexture;

namespace
{
    void generateMipMaps(const questart::VulkanDevice& device,
                         const questart::VulkanCommandPool& commandPool,
                         const questart::VulkanImage& image)
    {
        vk::ImageSubresourceRange barrierSubresourceRange{
            vk::ImageAspectFlagBits::eColor, // Aspect mask
            0,                               // Base mip level
            1,                               // Level count
            0,                               // Base array layer
            1};                              // Layer count

        vk::ImageMemoryBarrier barrier{
            vk::AccessFlags(),           // Source access mask
            vk::AccessFlags(),           // Destination access mask
            vk::ImageLayout::eUndefined, // Old layout
            vk::ImageLayout::eUndefined, // New layout
            VK_QUEUE_FAMILY_IGNORED,     // Source queue family index
            VK_QUEUE_FAMILY_IGNORED,     // Destination queue family index
            image.getImage(),            // Image
            barrierSubresourceRange};    // Subresource range

        vk::UniqueCommandBuffer commandBuffer{commandPool.beginCommandBuffer(device)};

        int32_t mipWidth{static_cast<int32_t>(image.getWidth())};
        int32_t mipHeight{static_cast<int32_t>(image.getHeight())};
        uint32_t mipLevels{image.getMipLevels()};

        for (uint32_t mipLevel = 1; mipLevel < mipLevels; mipLevel++)
        {
            barrier.subresourceRange.baseMipLevel = mipLevel - 1;
            barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
            barrier.newLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
            barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;

            commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                           vk::PipelineStageFlagBits::eTransfer,
                                           vk::DependencyFlags(),
                                           0, nullptr,
                                           0, nullptr,
                                           1, &barrier);

            vk::ImageSubresourceLayers sourceSubresource{
                vk::ImageAspectFlagBits::eColor, // Aspect mask
                mipLevel - 1,                    // Mip level
                0,                               // Base array layer
                1};                              // Layer count

            std::array<vk::Offset3D, 2> sourceOffsets{
                vk::Offset3D{0, 0, 0},
                vk::Offset3D{mipWidth, mipHeight, 1}};

            vk::ImageSubresourceLayers destinationSubresource{
                vk::ImageAspectFlagBits::eColor, // Aspect mask
                mipLevel,                        // Mip level
                0,                               // Base array layer
                1};                              // Layer count

            std::array<vk::Offset3D, 2> destinationOffsets{
                vk::Offset3D{0, 0, 0},
                vk::Offset3D{mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1}};

            vk::ImageBlit blit{
                sourceSubresource,      // Source subresource
                sourceOffsets,          // Source offsets
                destinationSubresource, // Destination subresource
                destinationOffsets};    // Destination offsets

            commandBuffer->blitImage(image.getImage(), vk::ImageLayout::eTransferSrcOptimal,
                                     image.getImage(), vk::ImageLayout::eTransferDstOptimal,
                                     1, &blit,
                                     vk::Filter::eLinear);

            barrier.oldLayout = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

            commandBuffer->pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                           vk::PipelineStageFlagBits::eFragmentShader,
                                           vk::DependencyFlags(),
                                           0, nullptr,
                                           0, nullptr,
                                           1, &barrier);

            if (mipWidth > 1)
            {
                mipWidth /= 2;
            }

            if (mipHeight > 1)
            {
                mipHeight /= 2;
            }
        }

        barrier.subresourceRange.baseMipLevel = mipLevels - 1;
        barrier.oldLayout = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

        commandBuffer->pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eFragmentShader,
            vk::DependencyFlags(),
            0, nullptr,
            0, nullptr,
            1, &barrier);

        commandPool.endCommandBuffer(commandBuffer.get(), device);
    }

    questart::VulkanImage createImage(const questart::VulkanPhysicalDevice& physicalDevice,
                                 const questart::VulkanDevice& device,
                                 const questart::VulkanCommandPool& commandPool,
                                 const questart::Bitmap& bitmap)
    {
        uint32_t imageWidth{bitmap.getWidth()};
        uint32_t imageHeight{bitmap.getHeight()};
        uint32_t mipLevels{static_cast<uint32_t>(std::floor(std::log2(std::max(imageWidth, imageHeight)))) + 1};
        vk::DeviceSize bufferSize{imageWidth * imageHeight * 4};

        questart::VulkanBuffer stagingBuffer{
            physicalDevice,
            device,
            bufferSize,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            bitmap.getPixelData()};

        questart::VulkanImage image{
            commandPool,
            physicalDevice,
            device,
            imageWidth,
            imageHeight,
            mipLevels,
            vk::SampleCountFlagBits::e1,
            vk::Format::eR8G8B8A8Unorm,
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eTransferSrc | vk::ImageUsageFlagBits::eSampled,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eTransferDstOptimal};

        vk::UniqueCommandBuffer commandBuffer{commandPool.beginCommandBuffer(device)};

        vk::ImageSubresourceLayers imageSubresource{
            vk::ImageAspectFlagBits::eColor, // Aspect mask
            0,                               // Mip level
            0,                               // Base array layer
            1};                              // Layer count

        vk::Extent3D imageExtent{
            imageWidth,  // Width
            imageHeight, // Height
            1};          // Depth

        vk::BufferImageCopy bufferImageCopy{
            0,                // Buffer offset
            0,                // Buffer row length
            0,                // Buffer image height
            imageSubresource, // Image subresource
            vk::Offset3D(),   // Image offset
            imageExtent};     // Image extent

        commandBuffer->copyBufferToImage(stagingBuffer.getBuffer(),
                                         image.getImage(),
                                         vk::ImageLayout::eTransferDstOptimal,
                                         1,
                                         &bufferImageCopy);

        commandPool.endCommandBuffer(commandBuffer.get(), device);

        ::generateMipMaps(device, commandPool, image);

        return image;
    }

    questart::VulkanImageView createImageView(const questart::VulkanDevice& device, const questart::VulkanImage& image)
    {
        return questart::VulkanImageView(device.getDevice(),
                                    image.getImage(),
                                    image.getFormat(),
                                    vk::ImageAspectFlagBits::eColor,
                                    image.getMipLevels());
    }

    vk::UniqueSampler createSampler(const questart::VulkanPhysicalDevice& physicalDevice,
                                    const questart::VulkanDevice& device,
                                    const questart::VulkanImage& image)
    {
        float maxLod{static_cast<float>(image.getMipLevels())};
        vk::Bool32 anisotropyEnabled = physicalDevice.isAnisotropicFilteringSupported() ? VK_TRUE : VK_FALSE;

        vk::SamplerCreateInfo info{
            vk::SamplerCreateFlags(),         // Flags
            vk::Filter::eLinear,              // Mag filter
            vk::Filter::eLinear,              // Min filter
            vk::SamplerMipmapMode::eLinear,   // Mipmap mode
            vk::SamplerAddressMode::eRepeat,  // Address mode U
            vk::SamplerAddressMode::eRepeat,  // Address mode V
            vk::SamplerAddressMode::eRepeat,  // Address mode W
            0.0f,                             // Mip LOD bias
            anisotropyEnabled,                // Anisotropy enabled
            anisotropyEnabled ? 8.0f : 1.0f,  // Max anisotropy
            VK_FALSE,                         // Compare enable
            vk::CompareOp::eNever,            // Compare op
            0.0f,                             // Min LOD
            maxLod,                           // Max LOD
            vk::BorderColor::eIntOpaqueBlack, // Border color
            VK_FALSE};                        // UnnormalizedCoordinates

        return device.getDevice().createSamplerUnique(info);
    }
} // namespace

struct VulkanTexture::Internal
{
    const questart::assets::Texture textureId;
    const questart::VulkanImage image;
    const questart::VulkanImageView imageView;
    const vk::UniqueSampler sampler;

    Internal(const questart::assets::Texture& textureId,
             const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanCommandPool& commandPool,
             const questart::Bitmap& bitmap)
        : textureId(textureId),
          image(::createImage(physicalDevice, device, commandPool, bitmap)),
          imageView(::createImageView(device, image)),
          sampler(::createSampler(physicalDevice, device, image)) {}
};

VulkanTexture::VulkanTexture(const questart::assets::Texture& textureId,
                             const questart::VulkanPhysicalDevice& physicalDevice,
                             const questart::VulkanDevice& device,
                             const questart::VulkanCommandPool& commandPool,
                             const questart::Bitmap& bitmap)
    : internal(questart::make_internal_ptr<Internal>(textureId,
                                                physicalDevice,
                                                device,
                                                commandPool,
                                                bitmap)) {}

const questart::assets::Texture& VulkanTexture::getTextureId() const
{
    return internal->textureId;
}

const questart::VulkanImageView& VulkanTexture::getImageView() const
{
    return internal->imageView;
}

const vk::Sampler& VulkanTexture::getSampler() const
{
    return internal->sampler.get();
}