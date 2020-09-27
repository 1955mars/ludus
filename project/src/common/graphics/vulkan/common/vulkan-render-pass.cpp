#include "vulkan-render-pass.hpp"


using questart::VulkanRenderPass;

namespace
{
    vk::UniqueRenderPass createRenderPass(const questart::VulkanPhysicalDevice& physicalDevice,
                                          const questart::VulkanDevice& device,
                                          const questart::VulkanSwapchain& swapchain)
    {
        vk::Format colorFormat{swapchain.getColorFormat()};
        vk::SampleCountFlagBits multiSamplingLevel{physicalDevice.getMultiSamplingLevel()};
        vk::Format depthFormat{physicalDevice.getDepthFormat()};

        vk::AttachmentDescription colorAttachment{
            vk::AttachmentDescriptionFlags(),          // Flags
            colorFormat,                               // Format
            multiSamplingLevel,                        // Samples
            vk::AttachmentLoadOp::eClear,              // Load operation
            vk::AttachmentStoreOp::eStore,             // Store operation
            vk::AttachmentLoadOp::eDontCare,           // Stencil load operation
            vk::AttachmentStoreOp::eDontCare,          // Stencil store operation
            vk::ImageLayout::eUndefined,               // Initial layout
            vk::ImageLayout::eColorAttachmentOptimal}; // Final layout

        vk::AttachmentReference colorAttachmentReference{
            0,                                         // Attachment index
            vk::ImageLayout::eColorAttachmentOptimal}; // Layout


        vk::AttachmentDescription depthTestingAttachment{
            vk::AttachmentDescriptionFlags(),                 // Flags
            depthFormat,                                      // Format
            multiSamplingLevel,                               // Samples
            vk::AttachmentLoadOp::eClear,                     // Load operation
            vk::AttachmentStoreOp::eDontCare,                 // Store operation
            vk::AttachmentLoadOp::eDontCare,                  // Stencil load operation
            vk::AttachmentStoreOp::eDontCare,                 // Stencil store operation
            vk::ImageLayout::eUndefined,                      // Initial layout
            vk::ImageLayout::eDepthStencilAttachmentOptimal}; // Final layout

        vk::AttachmentReference depthTestingAttachmentReference{
            1,                                                // Attachment index
            vk::ImageLayout::eDepthStencilAttachmentOptimal}; // Layout

        vk::AttachmentDescription multiSamplingAttachment{
            vk::AttachmentDescriptionFlags(), // Flags
            colorFormat,                      // Format
            vk::SampleCountFlagBits::e1,      // Samples
            vk::AttachmentLoadOp::eDontCare,  // Load operation
            vk::AttachmentStoreOp::eStore,    // Store operation
            vk::AttachmentLoadOp::eDontCare,  // Stencil load operation
            vk::AttachmentStoreOp::eDontCare, // Stencil store operation
            vk::ImageLayout::eUndefined,      // Initial layout
            vk::ImageLayout::ePresentSrcKHR}; // Final layout

        vk::AttachmentReference multiSamplingAttachmentReference{
            2,                                         // Attachment index
            vk::ImageLayout::eColorAttachmentOptimal}; // Layout


        std::array<vk::AttachmentDescription, 3> attachments{
            colorAttachment,
            depthTestingAttachment,
            multiSamplingAttachment};


        vk::SubpassDescription subpass{
            vk::SubpassDescriptionFlags(),     // Flags
            vk::PipelineBindPoint::eGraphics,  // Pipeline bind point
            0,                                 // Input attachment count
            nullptr,                           // Input attachments
            1,                                 // Color attachments count
            &colorAttachmentReference,         // Color attachments
            &multiSamplingAttachmentReference, // Resolve attachments
            &depthTestingAttachmentReference,  // Depth stencil attachments
            0,                                 // Preserve attachments count
            nullptr};                          // Preserve attachments


        vk::SubpassDependency subpassDependency{
            0,                                                                                    // Source subpass index
            0,                                                                                    // Destination subpass index
            vk::PipelineStageFlagBits::eColorAttachmentOutput,                                    // Source access mask
            vk::PipelineStageFlagBits::eColorAttachmentOutput,                                    // Destination access mask
            vk::AccessFlags(),                                                                    // Source access flags
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite, // Destination access flags
            vk::DependencyFlags()};                                                               // Dependency flags


        vk::RenderPassCreateInfo renderPassCreateInfo{
            vk::RenderPassCreateFlags(),               // Flags
            static_cast<uint32_t>(attachments.size()), // Attachment count
            attachments.data(),                        // Attachments
            1,                                         // Subpass count
            &subpass,                                  // Subpasses
            1,                                         // Dependency count
            &subpassDependency};                       // Dependencies

        return device.getDevice().createRenderPassUnique(renderPassCreateInfo);

    }
} // namespace

struct VulkanRenderPass::Internal
{
    const vk::UniqueRenderPass renderPass;

    Internal(const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanSwapchain& swapchain)
        : renderPass(::createRenderPass(physicalDevice, device, swapchain)) {}
};

VulkanRenderPass::VulkanRenderPass(const questart::VulkanPhysicalDevice& physicalDevice,
                                   const questart::VulkanDevice& device,
                                   const questart::VulkanSwapchain& swapchain)
    : internal(questart::make_internal_ptr<Internal>(physicalDevice, device, swapchain)) {}

const vk::RenderPass& VulkanRenderPass::getRenderPass() const
{
    return internal->renderPass.get();
}
