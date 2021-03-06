#include "vulkan-render-context.hpp"
#include "vulkan-render-pass.hpp"
#include "vulkan-image.hpp"
#include "vulkan-image-view.hpp"
#include "../../../core/log.hpp"

#if defined(QUEST)
#include "engine.hpp"
#include "../oculus/vulkan-swapchain.hpp"
#include "ovr-wrapper.hpp"
#else
    #include "../windows/vulkan-swapchain.hpp"
#endif

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

        uint32_t layerCount = 1;
#if defined(QUEST)
        layerCount = 2;
#endif

        return questart::VulkanImage(
            commandPool,
            physicalDevice,
            device,
            extent.width,
            extent.height,
            0,
            1,
            layerCount,
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

        uint32_t layerCount = 1;
#if defined(QUEST)
        layerCount = 2;
#endif

        return questart::VulkanImage(
            commandPool,
            physicalDevice,
            device,
            extent.width,
            extent.height,
            0,
            1,
            layerCount,
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
        uint32_t layerCount = 1;
#if defined(QUEST)
        layerCount = 2;
#endif
        return questart::VulkanImageView(device.getDevice(),
                                    image.getImage(),
                                    image.getFormat(),
                                    aspectFlags,
                                    image.getMipLevels(),
                                    layerCount,
                                    1);
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
    
    vk::Rect2D createScissor(const questart::VulkanSwapchain& swapchain)
    {
        vk::Offset2D offset{0, 0};

        return vk::Rect2D{
            offset,
            swapchain.getExtent()};
    }

    vk::Viewport createViewport(const questart::VulkanSwapchain& swapchain)
    {
        const vk::Extent2D extent{swapchain.getExtent()};
        const float viewportWidth{static_cast<float>(extent.width)};
        const float viewportHeight{static_cast<float>(extent.height)};

        return vk::Viewport{
            0.0f,           // X
            0.0f,           // Y
            viewportWidth,  // Width
            viewportHeight, // Height
            0.0f,           // Min depth
            1.0f};          // Max depth
    }

    std::array<vk::ClearValue, 2> createClearValues()
    {
        vk::ClearValue color;
        color.color = vk::ClearColorValue(std::array<float, 4>{
            1.0f, // Red
            1.0f,  // Green
            1.0f,  // Blue
            1.0f});          // Alpha

        vk::ClearValue depth;
        depth.depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

        return std::array<vk::ClearValue, 2>{color, depth};
    }

    uint32_t acquireNextImageIndex(const vk::Device& device,
                                   const vk::SwapchainKHR& swapchain,
                                   const vk::Fence& fence,
                                   const vk::Semaphore& semaphore)
    {
        static constexpr uint64_t timeOut{std::numeric_limits<uint64_t>::max()};

        device.waitForFences(
            1,        // Number of fences to wait for
            &fence,   // Fences to wait for
            VK_TRUE,  // Wait for all fences
            timeOut); // Timeout while waiting

        // The fence should now be reset, ready for the next use case.
        device.resetFences(1, &fence);

        vk::ResultValue nextImageIndex{device.acquireNextImageKHR(
            swapchain, // Swapchain to acquire from
            timeOut,   // Timeout while waiting
            semaphore, // Which semaphore to signal
            nullptr)}; // Which fence to signal

        return nextImageIndex.value;
    }

} // namespace

struct VulkanRenderContext::Internal
{
    const questart::VulkanSwapchain swapchain;
    const questart::VulkanRenderPass renderPass;
    const questart::VulkanImage multiSampleImage;
    const questart::VulkanImageView multiSampleImageView;
    const questart::VulkanImage depthImage;
    const questart::VulkanImageView depthImageView;
    const std::vector<vk::UniqueFramebuffer> framebuffers;
    const std::vector<vk::UniqueCommandBuffer> commandBuffers;
    const uint32_t maxRenderFrames;
    const std::vector<vk::UniqueSemaphore> graphicsSemaphores;
    const std::vector<vk::UniqueSemaphore> presentationSemaphores;
    const std::vector<vk::UniqueFence> graphicsFences;
    const vk::Rect2D scissor;
    const vk::Viewport viewport;
    const std::array<vk::ClearValue, 2> clearValues;
    uint32_t currentFrameIndex{0};
    uint32_t currentSwapchainImageIndex{0};

    Internal(const vk::Instance& instance,
             const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanCommandPool& commandPool,
             const vk::SwapchainKHR& oldSwapchain)
        : swapchain(questart::VulkanSwapchain(instance, physicalDevice, device, commandPool, oldSwapchain)),
          renderPass(questart::VulkanRenderPass(physicalDevice, device, swapchain)),
          multiSampleImage(::createMultiSampleImage(physicalDevice, device, swapchain, commandPool)),
          multiSampleImageView(::createImageView(device, multiSampleImage, vk::ImageAspectFlagBits::eColor)),
          depthImage(::createDepthImage(commandPool, physicalDevice, device, swapchain)), 
          depthImageView(::createImageView(device, depthImage, vk::ImageAspectFlagBits::eDepth)),
          framebuffers(::createFramebuffers(device, swapchain, renderPass, multiSampleImageView, depthImageView)),
          commandBuffers(commandPool.createCommandBuffers(device, swapchain.getImageCount())),
          maxRenderFrames(swapchain.getImageCount()),
          graphicsSemaphores(device.createSemaphores(maxRenderFrames)),
          presentationSemaphores(device.createSemaphores(maxRenderFrames)),
          graphicsFences(device.createFences(maxRenderFrames)),
          scissor(::createScissor(swapchain)),
          viewport(::createViewport(swapchain)),
          clearValues(::createClearValues())
    {
        questart::log("RenderContext: ", "maxRenderFrames: " + std::to_string(swapchain.getImageCount()));
    }

    bool renderBegin(const questart::VulkanDevice& device)
    {

        currentFrameIndex = (currentFrameIndex + 1) % maxRenderFrames;

        // Get the appropriate graphics fence and semaphore for the current render frame.
        const vk::Fence& graphicsFence{graphicsFences[currentFrameIndex].get()};

#if defined(QUEST)
        // Wait for the fence to complete and then reset it
        static constexpr uint64_t timeout{std::numeric_limits<uint64_t>::max()};
        device.getDevice().waitForFences(1, &graphicsFence, VK_TRUE, timeout);
        device.getDevice().resetFences(1, &graphicsFence);

        currentSwapchainImageIndex = currentFrameIndex;
#else

        const vk::Semaphore& graphicsSemaphore{graphicsSemaphores[currentFrameIndex].get()};

        try
        {
            // Attempt to acquire the next swapchain image index to target.
            currentSwapchainImageIndex = ::acquireNextImageIndex(device.getDevice(),
                                                                 swapchain.getSwapchain(),
                                                                 graphicsFence,
                                                                 graphicsSemaphore);
        }
        catch (vk::OutOfDateKHRError outOfDateError)
        {
            questart::log("RenderBegin", "OutofDate SwapChain Error!");
            // We cannot render with the current swapchain - it needs to be recreated.
            return false;
        }

#endif
        // Grab the command buffer to use for the current swapchain image index.
        const vk::CommandBuffer& commandBuffer{getActiveCommandBuffer()};

        // Reset the command buffer to a fresh state.
        commandBuffer.reset(vk::CommandBufferResetFlagBits::eReleaseResources);

        // Begin the command buffer.
        vk::CommandBufferBeginInfo commandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr};
        commandBuffer.begin(&commandBufferBeginInfo);
        // Configure the scissor.
        commandBuffer.setScissor(
            0,         // Which scissor to start at
            1,         // How many scissors to apply
            &scissor); // Scissor data

        // Configure the viewport.
        commandBuffer.setViewport(
            0,          // Which viewport to start at
            1,          // How many viewports to apply
            &viewport); // Viewport data

        // Define the render pass attributes to apply.
        vk::RenderPassBeginInfo renderPassBeginInfo{
            renderPass.getRenderPass(),                     // Render pass to use
            framebuffers[currentFrameIndex].get(), // Current frame buffer
            scissor,                                        // Render area
            2,                                              // Clear value count
            clearValues.data()};                            // Clear values

        // Record the begin render pass command.
        commandBuffer.beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);

        return true;

    }

    bool renderEnd(const questart::VulkanDevice& device, const double& displayTime, void* tracking, long long frameIndex)
    {
        // Grab the command buffer to use for the current swapchain image index.
        const vk::CommandBuffer& commandBuffer{getActiveCommandBuffer()};

        // Request the command buffer to end its recording phase.
        commandBuffer.endRenderPass();
        commandBuffer.end();

        // Get the appropriate graphics fence and semaphores for the current render frame.
        const vk::Fence& graphicsFence{graphicsFences[currentFrameIndex].get()};
#ifndef QUEST
        const vk::Semaphore& graphicsSemaphore{graphicsSemaphores[currentFrameIndex].get()};
        const vk::Semaphore& presentationSemaphore{presentationSemaphores[currentFrameIndex].get()};
#endif
        const vk::PipelineStageFlags pipelineStageFlags{vk::PipelineStageFlagBits::eColorAttachmentOutput};

#if defined(QUEST)
        // Build a submission object for the graphics queue to process.
        vk::SubmitInfo submitInfo{
                0,                       // Wait semaphore count
                nullptr,      // Wait semaphores
                &pipelineStageFlags,     // Pipeline stage flags
                1,                       // Command buffer count
                &commandBuffer,          // Command buffer
                0,                       // Signal semaphore count
                nullptr}; // Signal semaphores
#else
        // Build a submission object for the graphics queue to process.
        vk::SubmitInfo submitInfo{
            1,                       // Wait semaphore count
            &graphicsSemaphore,      // Wait semaphores
            &pipelineStageFlags,     // Pipeline stage flags
            1,                       // Command buffer count
            &commandBuffer,          // Command buffer
            1,                       // Signal semaphore count
            &presentationSemaphore}; // Signal semaphores
#endif
        // Submit our command buffer and configuration to the graphics queue.
        device.getGraphicsQueue().submit(1, &submitInfo, graphicsFence);

#if defined (QUEST)



        ovrTracking2* trackingInfo = (ovrTracking2*) tracking;

        if(tracking == nullptr)
        {
            questart::log("Render End:" , "Tracking info is null");
        }

        //questart::log("Render End: ", "Tracking info: " + std::to_string(trackingInfo->Eye[0].ViewMatrix.M[0][0]));

        ovrLayerProjection2 worldLayer = vrapi_DefaultLayerProjection2();
        worldLayer.HeadPose = trackingInfo->HeadPose;
        for(int eye=0; eye < VRAPI_FRAME_LAYER_EYE_MAX; eye++)
        {
            worldLayer.Textures[eye].ColorSwapChain = swapchain.getSwapChain();
            worldLayer.Textures[eye].SwapChainIndex = currentFrameIndex;
            worldLayer.Textures[eye].TexCoordsFromTanAngles = ovrMatrix4f_TanAngleMatrixFromProjection(&trackingInfo->Eye[eye].ProjectionMatrix);
        }

        const ovrLayerHeader2* layers[] = {&worldLayer.Header};

        ovrSubmitFrameDescription2 frameDesc{0};
        frameDesc.Flags = 0;
        frameDesc.SwapInterval = 1;
        frameDesc.FrameIndex = frameIndex;
        frameDesc.DisplayTime = displayTime;
        frameDesc.LayerCount = 1;
        frameDesc.Layers = layers;

        vrapi_SubmitFrame2(questart::Engine::ovrApp, &frameDesc);
#else
        // Construct an info object to describe what to present to the screen.
        vk::PresentInfoKHR presentationInfo{
            1,                           // Semaphore count
            &presentationSemaphore,      // Wait semaphore
            1,                           // Swapchain count
            &swapchain.getSwapchain(),   // Swapchain
            &currentSwapchainImageIndex, // Image indices
            nullptr};                    // Results

        try
        {
            // Attempt to submit our graphics output to the presentation queue for display.
            // If we receive an out of date error, or the result comes back as sub optimal
            // we will return false as it indicates our swapchain should be recreated.
            if (device.getPresentationQueue().presentKHR(presentationInfo) == vk::Result::eSuboptimalKHR)
            {
                return false;
            }
        }
        catch (vk::OutOfDateKHRError outOfDateError)
        {
            return false;
        }

        // We now wait for the presentation to have been completed before continuing.
        device.getPresentationQueue().waitIdle();
#endif
        // Increment our current frame index, wrapping it when it hits our maximum.

        return true;

    }

    const vk::CommandBuffer& getActiveCommandBuffer() const
    {
        return commandBuffers[currentFrameIndex].get();
    }
};

VulkanRenderContext::VulkanRenderContext(const vk::Instance& instance,
                                         const questart::VulkanPhysicalDevice& physicalDevice,
                                         const questart::VulkanDevice& device,
                                         const questart::VulkanCommandPool& commandPool,
                                         const vk::SwapchainKHR& oldSwapchain)
    : internal(questart::make_internal_ptr<Internal>(instance, physicalDevice, device, commandPool, oldSwapchain)) {}

bool VulkanRenderContext::renderBegin(const questart::VulkanDevice& device)
{
    return internal->renderBegin(device);
}

bool VulkanRenderContext::renderEnd(const questart::VulkanDevice& device, const double& displayTime, void* tracking, long long frameIndex)
{
    return internal->renderEnd(device, displayTime, tracking, frameIndex);
}
/*
questart::VulkanRenderContext VulkanRenderContext::recreate(const vk::Instance& instance,
                                                       const questart::VulkanPhysicalDevice& physicalDevice,
                                                       const questart::VulkanDevice& device,
                                                       const questart::VulkanCommandPool& commandPool)
{
    return questart::VulkanRenderContext(instance,
                                    physicalDevice,
                                    device,
                                    commandPool,
                                    internal->swapchain.getSwapchain());
}
*/
const vk::Viewport& VulkanRenderContext::getViewport() const
{
    return internal->viewport;
}

const vk::Rect2D& VulkanRenderContext::getScissor() const
{
    return internal->scissor;
}

const vk::RenderPass& VulkanRenderContext::getRenderPass() const
{
    return internal->renderPass.getRenderPass();
}

const vk::CommandBuffer& VulkanRenderContext::getActiveCommandBuffer() const
{
    return internal->getActiveCommandBuffer();
}

const questart::WindowSize& VulkanRenderContext::getCurrentWindowSize()
{
    return internal->swapchain.getCurrentWindowSize();
}