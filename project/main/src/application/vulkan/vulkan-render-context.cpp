#include "vulkan-render-context.hpp"
#include "vulkan-swapchain.hpp"
#include "vulkan-render-pass.hpp"

using questart::VulkanRenderContext;

struct VulkanRenderContext::Internal
{
    const questart::VulkanSwapchain swapchain;
    const questart::VulkanRenderPass renderPass;

    Internal(const questart::SDLWindow& window,
             const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanSurface& surface)
        : swapchain(questart::VulkanSwapchain(window, physicalDevice, device, surface)),
          renderPass(questart::VulkanRenderPass(physicalDevice, device, swapchain)) {}
};

VulkanRenderContext::VulkanRenderContext(const questart::SDLWindow& window,
                                         const questart::VulkanPhysicalDevice& physicalDevice,
                                         const questart::VulkanDevice& device,
                                         const questart::VulkanSurface& surface)
    : internal(questart::make_internal_ptr<Internal>(window, physicalDevice, device, surface)) {}