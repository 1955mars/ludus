#include "vulkan-render-context.hpp"
#include "vulkan-swapchain.hpp"
#include "vulkan-render-pass.hpp"
#include "vulkan-image.hpp"
#include "vulkan-image-view.hpp"

#include <vector>

using questart::VulkanRenderContext;

namespace
{
    questart::VulkanImage createMultiSampleImage(const questart::VulkanPhysicalDevice& physicalDevice,
                                            const questart::VulkanDevice& device,
                                            const questart::VulkanSwapchain& swapchain,
                                            const questart::VulkanCommandPool& commandPool)
    {
        const vk::Extent2D& extent{swapchain.getExtent()};

        return questart::VulkanImage(
            commandPool,
            physicalDevice,
            device,
            extent.width,
            extent.height,
            1,
            physicalDevice.getMultiSamplingLevel(),
            swapchain.getColorFormat(),
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eColorAttachmentOptimal);
    }

    questart::VulkanImage createDepthImage(const questart::VulkanCommandPool& commandPool,
                                      const questart::VulkanPhysicalDevice& physicalDevice,
                                      const questart::VulkanDevice& device,
                                      const questart::VulkanSwapchain& swapchain)
    {
        const vk::Extent2D& extent{swapchain.getExtent()};

        return questart::VulkanImage(
            commandPool,
            physicalDevice,
            device,
            extent.width,
            extent.height,
            1,
            physicalDevice.getMultiSamplingLevel(),
            physicalDevice.getDepthFormat(),
            vk::ImageTiling::eOptimal,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            vk::ImageLayout::eUndefined,
            vk::ImageLayout::eDepthStencilAttachmentOptimal);
    }


    questart::VulkanImageView createImageView(const questart::VulkanDevice& device,
                                         const questart::VulkanImage& image,
                                         const vk::ImageAspectFlags& aspectFlags)
    {
        return questart::VulkanImageView(device.getDevice(),
                                    image.getImage(),
                                    image.getFormat(),
                                    aspectFlags,
                                    image.getMipLevels());
    }

    std::vector<vk::UniqueFramebuffer> createFramebuffers(const questart::VulkanDevice& device,
                                                          const questart::VulkanSwapchain& swapchain,
                                                          const questart::VulkanRenderPass& renderPass,
                                                          const questart::VulkanImageView& multiSampleImageView,
                                                          const questart::VulkanImageView& depthImageView)
    {
        std::vector<vk::UniqueFramebuffer> framebuffers;

        const vk::Extent2D& extent{swapchain.getExtent()};

        for (const auto& swapchainImageView : swapchain.getImageViews())
        {
            std::array<vk::ImageView, 3> attachments{
                multiSampleImageView.getImageView(),
                depthImageView.getImageView(),
                swapchainImageView.getImageView()};

            vk::FramebufferCreateInfo info{
                vk::FramebufferCreateFlags(),              // Flags
                renderPass.getRenderPass(),                // Render pass
                static_cast<uint32_t>(attachments.size()), // Attachment count
                attachments.data(),                        // Attachments
                extent.width,                              // Width
                extent.height,                             // Height
                1};                                        // Layers

            framebuffers.push_back(device.getDevice().createFramebufferUnique(info));
        }

        return framebuffers;
    }
}

struct VulkanRenderContext::Internal
{
    const questart::VulkanSwapchain swapchain;
    const questart::VulkanRenderPass renderPass;
    const questart::VulkanImage multiSampleImage;
    const questart::VulkanImageView multiSampleImageView;
    const questart::VulkanImage depthImage;
    const questart::VulkanImageView depthImageView;
    const std::vector<vk::UniqueFramebuffer> framebuffers;

    Internal(const questart::SDLWindow& window,
             const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanSurface& surface,
             const questart::VulkanCommandPool& commandPool)
        : swapchain(questart::VulkanSwapchain(window, physicalDevice, device, surface)),
          renderPass(questart::VulkanRenderPass(physicalDevice, device, swapchain)),
          multiSampleImage(::createMultiSampleImage(physicalDevice, device, swapchain, commandPool)),
          multiSampleImageView(::createImageView(device, multiSampleImage, vk::ImageAspectFlagBits::eColor)),
          depthImage(::createDepthImage(commandPool, physicalDevice, device, swapchain)), 
          depthImageView(::createImageView(device, depthImage, vk::ImageAspectFlagBits::eDepth)),
          framebuffers(::createFramebuffers(device, swapchain, renderPass, multiSampleImageView, depthImageView)) {}
};

VulkanRenderContext::VulkanRenderContext(const questart::SDLWindow& window,
                                         const questart::VulkanPhysicalDevice& physicalDevice,
                                         const questart::VulkanDevice& device,
                                         const questart::VulkanSurface& surface,
                                         const questart::VulkanCommandPool& commandPool)
    : internal(questart::make_internal_ptr<Internal>(window, physicalDevice, device, surface, commandPool)) {}