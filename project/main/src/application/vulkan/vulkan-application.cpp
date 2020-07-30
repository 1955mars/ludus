#include "vulkan-application.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "vulkan-context.hpp"

using questart::VulkanApplication;

struct VulkanApplication::Internal
{
    const questart::VulkanContext context;

    Internal() : context(questart::VulkanContext()) {}

    void update(const float& delta) {}

    void render() {}
};

VulkanApplication::VulkanApplication() : internal(questart::make_internal_ptr<Internal>()) {}

void VulkanApplication::update(const float& delta)
{
    internal->update(delta);
}

void VulkanApplication::render()
{
    internal->render();
}