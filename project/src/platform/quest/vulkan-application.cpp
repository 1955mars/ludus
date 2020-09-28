#include "vulkan-application.hpp"
#include "vulkan-context.hpp"
#if defined(QUEST)
#include "scene-ovr.hpp"
#else
#include "scene-main.hpp"
#endif

using questart::VulkanApplication;

namespace
{
    std::unique_ptr<questart::Scene> createMainScene(questart::VulkanContext& context)
    {
        const questart::WindowSize& size = context.getCurrentWindowSize();
        questart::WindowSize wSize{
            size.width,
            size.height};
#if defined(QUEST)
        std::unique_ptr<questart::Scene> scene{std::make_unique<questart::SceneOVR>()};
#else
        std::unique_ptr<questart::Scene> scene{std::make_unique<questart::SceneMain>(wSize)};
#endif
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

    void update(const float& delta, void* tracking)
    {
        getScene().update(delta, tracking);
    }

    void render(const double& displayTime, void* tracking, long long frameIndex)
    {
        if (context.renderBegin())
        {
            getScene().render(context);
            context.renderEnd(displayTime, tracking, frameIndex);
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
    internal->update(delta, tracking);
}

void VulkanApplication::render(const double& displayTime, void* tracking, long long frameIndex)
{
    internal->render(displayTime, tracking, frameIndex);
}
