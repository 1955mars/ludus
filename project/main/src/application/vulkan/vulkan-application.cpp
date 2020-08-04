#include "vulkan-application.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/sdl-wrapper.hpp"
#include "../../scene/scene-main.hpp"
#include "vulkan-context.hpp"

using questart::VulkanApplication;

namespace
{
    std::unique_ptr<questart::Scene> createMainScene(questart::VulkanContext& context)
    {
        std::pair<uint32_t, uint32_t> displaySize{questart::sdl::getDisplaySize()};
        std::unique_ptr<questart::Scene> scene{std::make_unique<questart::SceneMain>(
            static_cast<float>(displaySize.first),
            static_cast<float>(displaySize.second))};

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