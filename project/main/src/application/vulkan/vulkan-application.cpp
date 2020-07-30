#include "vulkan-application.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/sdl-wrapper.hpp"
#include "vulkan-context.hpp"

using questart::VulkanApplication;

struct VulkanApplication::Internal
{
    const questart::VulkanContext context;
    SDL_Window* window;

    Internal() : context(questart::VulkanContext()),
                 window(questart::sdl::createWindow(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)) {}

    void update(const float& delta) {}

    void render() {}

    ~Internal()
    {
        if (window)
        {
            SDL_DestroyWindow(window);
        }
    }
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