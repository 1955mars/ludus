#include "vulkan-swapchain.hpp"
#include "../common/vulkan-command-pool.hpp"
#include "../../../core/log.hpp"
#include "ovr-wrapper.hpp"

#include "engine.hpp"
#include "../../../../platform/quest/engine.hpp"

using questart::VulkanSwapchain;


namespace
{
    struct VulkanSwapchainFormat
    {
        vk::ColorSpaceKHR colorSpace;
        vk::Format colorFormat;
    };

    void createVrApiSystemVulkan(const vk::Instance& instance, const questart::VulkanPhysicalDevice& physicalDevice,
                                 const questart::VulkanDevice& device)
    {
        questart::log("questart::createVrApiSystemVulkan::", "Entering!");
        ovrSystemCreateInfoVulkan systemInfo;
        systemInfo.Instance = *reinterpret_cast<const VkInstance*>(&instance);
        systemInfo.PhysicalDevice = *reinterpret_cast<const VkPhysicalDevice*>(&physicalDevice.getPhysicalDevice());;
        systemInfo.Device = *reinterpret_cast<const VkDevice*>(&device.getDevice());
        ovrResult result = vrapi_CreateSystemVulkan(&systemInfo);
        if( result != ovrSuccess)
        {
            questart::log("questart::createVrApiSystemVulkan::", "Failed to create VrApi system vulkan");
            throw std::runtime_error("Failed to create VrApi system vulkan!");
        }
        questart::log("questart::createVrApiSystemVulkan::", "Created VrApi System Successfully!");
    }

    ovrTextureSwapChain* createSwapChain(const vk::Instance& instance, const questart::VulkanPhysicalDevice& physicalDevice,
                                         const questart::VulkanDevice& device)
    {
        questart::log("questart::createOVRTextureSwapChain: ", "Entering");

        createVrApiSystemVulkan(instance, physicalDevice, device);

        uint32_t width = vrapi_GetSystemPropertyInt(&questart::Engine::appJava, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);
        uint32_t height = vrapi_GetSystemPropertyInt(&questart::Engine::appJava, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);

        return vrapi_CreateTextureSwapChain3(
                VRAPI_TEXTURE_TYPE_2D_ARRAY,
                VK_FORMAT_R8G8B8A8_UNORM,
                width,
                height,
                1,
                3
        );
    }

    vk::Extent2D getExtent()
    {
        uint32_t width = vrapi_GetSystemPropertyInt(&questart::Engine::appJava, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_WIDTH);
        uint32_t height = vrapi_GetSystemPropertyInt(&questart::Engine::appJava, VRAPI_SYS_PROP_SUGGESTED_EYE_TEXTURE_HEIGHT);

        questart::log("Get Extent: width = " , std::to_string(width) + " height = " + std::to_string(height));

        return vk::Extent2D{
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)};
    }

    void applyTransitionLayoutCommand(const questart::VulkanDevice& device,
                                      const questart::VulkanCommandPool& commandPool,
                                      const vk::PipelineStageFlags& sourceStageFlags,
                                      const vk::PipelineStageFlags& destinatinStageFlags,
                                      const vk::ImageMemoryBarrier& barrier)
    {
        //questart::log("Debug", "applyTransitionLayoutCommand");
        vk::UniqueCommandBuffer  commandBuffer = commandPool.beginCommandBuffer(device);

        //questart::log("Debug", "before pipelinebarrier");
        commandBuffer->pipelineBarrier(
                sourceStageFlags,
                destinatinStageFlags,
                vk::DependencyFlags(),
                0, nullptr,
                0, nullptr,
                1, &barrier);

        commandPool.endCommandBuffer(commandBuffer.get(), device);
    }


    void transitionLayout(const questart::VulkanDevice& device,
                          const questart::VulkanCommandPool& commandPool,
                          const vk::Image& image,
                          const uint32_t& mipLevels,
                          const uint32_t& layerCount,
                          const vk::ImageLayout& oldLayout,
                          const vk::ImageLayout& newLayout)
    {
        //questart::log("Debug", "TransitionLayout1");
        vk::ImageMemoryBarrier barrier;
        barrier.image = image;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = layerCount;

        // Scenario: undefined -> shader read only optimal
        if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
        {
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

            ::applyTransitionLayoutCommand(device,
                                           commandPool,
                                           vk::PipelineStageFlagBits::eTopOfPipe,
                                           vk::PipelineStageFlagBits::eAllGraphics,
                                           barrier);
            //questart::log("Debug", "TransitionLayout1 leaving");
            return;
        }

        // Scenario: undefined -> fragment density optimal ext
        if(oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eFragmentDensityMapOptimalEXT)
        {
            barrier.dstAccessMask = vk::AccessFlagBits::eFragmentDensityMapReadEXT;
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;

            return ::applyTransitionLayoutCommand(device,
                                                  commandPool,
                                                  vk::PipelineStageFlagBits::eTopOfPipe,
                                                  vk::PipelineStageFlagBits::eAllGraphics,
                                                  barrier);
        }
    }



    std::vector<vk::Image> getImages(ovrTextureSwapChain* swapChain, int swapChainLength)
    {
        questart::log("questart::getImages: ", "Entering");
        std::vector<vk::Image> images;
        for(int i=0; i<swapChainLength; i++)
        {
            images.push_back(vrapi_GetTextureSwapChainBufferVulkan(swapChain, i));
        }
        return images;
    }

    std::vector<questart::VulkanImageView> createImageViews(const questart::VulkanDevice& device,
                                                            std::vector<vk::Image>& images,
                                                            const VulkanSwapchainFormat& format)
    {
        std::vector<questart::VulkanImageView> imageViews;

        // For each of the images in the swap chain, we need to create a new 'image view'.
        for (const vk::Image& image : images)
        {
            questart::VulkanImageView imageView{
                    device.getDevice(),
                    image,
                    format.colorFormat,
                    vk::ImageAspectFlagBits::eColor,
                    1,
                    2, // LayerCount = 2 if MultiView
                    1};

            imageViews.push_back(std::move(imageView));
        }

        return imageViews;
    }

    std::vector<vk::Image> getFragmentDensityTextures(ovrTextureSwapChain* swapChain, std::vector<vk::Extent2D>& textureSizes)
    {
        questart::log("questart::getFragmentDensityTextures: ", "Entering");
        std::vector<vk::Image> densityTextures;
        for(int i=0; i<textureSizes.size(); i++)
        {
            VkImage image;
            ovrResult result =  vrapi_GetTextureSwapChainBufferFoveationVulkan(
                    swapChain,
                    i,
                    &image,
                    &textureSizes[i].width,
                    &textureSizes[i].height
            );
            densityTextures.emplace_back(image);
        }
        return densityTextures;
    }

    std::vector<vk::Extent2D> getFragmentDensityTextureSizes(int swapChainLength)
    {
        questart::log("questart::getFragmentDensityTextureSizes: ", "Entering");
        std::vector<vk::Extent2D> textureSizes;
        for(int i=0; i<swapChainLength; i++)
        {
            textureSizes.emplace_back();
        }
        return textureSizes;
    }

}


struct VulkanSwapchain::Internal
{
    ovrTextureSwapChain* swapChain;
    int swapChainLength;
    const VulkanSwapchainFormat format;
    const vk::Extent2D extent;
    std::vector<vk::Image> images;
    std::vector<questart::VulkanImageView> imageViews;
    std::vector<vk::Extent2D> fragmentDensityTextureSizes;
    std::vector<vk::Image> fragmentDensityTextures;
    Internal(const vk::Instance& instance,
             const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanCommandPool& commandPool,
             const vk::SwapchainKHR& oldSwapchain)
            : swapChain(::createSwapChain(instance, physicalDevice, device)),
              swapChainLength(vrapi_GetTextureSwapChainLength(swapChain)),
              format(VulkanSwapchainFormat{vk::ColorSpaceKHR::eSrgbNonlinear, vk::Format::eR8G8B8A8Unorm}),
              extent(::getExtent()),
              images(::getImages(swapChain, swapChainLength)),
              fragmentDensityTextureSizes(::getFragmentDensityTextureSizes(swapChainLength)),
              fragmentDensityTextures(::getFragmentDensityTextures(swapChain, fragmentDensityTextureSizes))
    {
        questart::log("questart::swapchain", "swapChainLength: " + std::to_string(images.size()));
        questart::log("questart::swapchain", "fragmentWidth: " + std::to_string(fragmentDensityTextureSizes[0].width));
        questart::log("questart::swapchain", "fragmentHeight: " + std::to_string(fragmentDensityTextureSizes[0].height));

        // Describing the layout transition for images obtained from swapchain
        for(const auto& image : images)
        {
            ::transitionLayout(device, commandPool, image, 1, 2, vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eShaderReadOnlyOptimal);
        }

        for(const auto& image : fragmentDensityTextures)
        {
            ::transitionLayout(device, commandPool, image, 1, 2, vk::ImageLayout::eUndefined,
                               vk::ImageLayout::eFragmentDensityMapOptimalEXT);
        }

        imageViews = ::createImageViews(device, images, format);
    }
};

VulkanSwapchain::VulkanSwapchain(const vk::Instance& instance,
                const questart::VulkanPhysicalDevice& physicalDevice,
                const questart::VulkanDevice& device,
                const questart::VulkanCommandPool& commandPool,
                const vk::SwapchainKHR& oldSwapchain)
                :internal(make_internal_ptr<Internal>(instance, physicalDevice, device, commandPool, oldSwapchain))
{
}



ovrTextureSwapChain* VulkanSwapchain::getSwapChain() const
{
    return internal->swapChain;
}

const vk::Format& VulkanSwapchain::getColorFormat() const
{
    return internal->format.colorFormat;
}

const vk::Extent2D& VulkanSwapchain::getExtent() const
{
    return internal->extent;
}

uint32_t VulkanSwapchain::getImageCount() const
{
    return static_cast<uint32_t>(internal->imageViews.size());
}

const std::vector<questart::VulkanImageView>& VulkanSwapchain::getImageViews() const
{
    return internal->imageViews;
}

const questart::WindowSize& VulkanSwapchain::getCurrentWindowSize() const
{
    return questart::WindowSize{internal->extent.width, internal->extent.height};
}