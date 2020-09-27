#include "vulkan-surface.hpp"

using questart::VulkanSurface;

namespace
{
    vk::UniqueSurfaceKHR createSurface(const vk::Instance& instance,
                                       const questart::VulkanPhysicalDevice& physicalDevice,
                                       const questart::SDLWindow& window)
    {
        static const std::string logTag{"questart::VulkanSurface::createSurface"};

        VkSurfaceKHR sdlSurface;

        // Ask SDL to create a Vulkan surface from its window.
        if (!SDL_Vulkan_CreateSurface(window.getWindow(), instance, &sdlSurface))
        {
            throw std::runtime_error(logTag + ": SDL could not create a Vulkan surface.");
        }

        // Wrap the result in a Vulkan managed surface object.
        return vk::UniqueSurfaceKHR{sdlSurface, instance};
    }
} // namespace

struct VulkanSurface::Internal
{
    const vk::UniqueSurfaceKHR surface;

    Internal(const vk::Instance& instance,
             const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::SDLWindow& window)
        : surface(::createSurface(instance, physicalDevice, window)) {}
};

VulkanSurface::VulkanSurface(const vk::Instance& instance,
                             const questart::VulkanPhysicalDevice& physicalDevice,
                             const questart::SDLWindow& window)
    : internal(questart::make_internal_ptr<Internal>(instance, physicalDevice, window)) {}

const vk::SurfaceKHR& VulkanSurface::getSurface() const
{
    return *internal->surface;
}