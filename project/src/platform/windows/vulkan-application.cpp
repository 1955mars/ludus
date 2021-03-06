#include "vulkan-application.hpp"
#include "../../../core/graphics-wrapper.hpp"
#include "scene-main.hpp"
#include "vulkan-context.hpp"

using questart::VulkanApplication;

namespace
{
    std::unique_ptr<questart::Scene> createMainScene(questart::VulkanContext& context)
    {
        const questart::WindowSize& size = context.getCurrentWindowSize();
        questart::WindowSize wSize{
            size.width,
            size.height};
       
        std::unique_ptr<questart::Scene> scene{std::make_unique<questart::SceneMain>(wSize)};
        context.loadAssetManifest(scene->getAssetManifest());
        scene->prepare();

        return scene;
    }
} // namespace

struct VulkanApplication::Internal
{
    questart::VulkanContext context;
    std::unique_ptr<questart::Scene> scene;

    Internal() : context(questart::VulkanContext()) {}

    questart::Scene& getScene()
    {
        if (!scene)
        {
            scene = ::createMainScene(context);
        }

        return *scene;
    }

    void update(const float& delta)
    {
        getScene().update(delta);
    }

    void render()
    {
        if (context.renderBegin())
        {
            getScene().render(context);
            context.renderEnd();
        }
    }

    void onWindowResized()
    {
        const questart::WindowSize& wSize = context.getCurrentWindowSize();
        questart::WindowSize size{
            wSize.width,
            wSize.height};
        getScene().onWindowResized(size);
    }
};

VulkanApplication::VulkanApplication() : internal(questart::make_internal_ptr<Internal>()) {}

void VulkanApplication::update(const float& delta, void* tracking)
{
    internal->update(delta);
}

void VulkanApplication::render(const double& displayTime, void* tracking, long long frameIndex)
{
    internal->render();
}

void VulkanApplication::onWindowResized()
{
    internal->onWindowResized();
}