#include "vulkan-context.hpp"
#include "vulkan-common.hpp"
#include "vulkan-physical-device.hpp"
#include "vulkan-surface.hpp"
#include "vulkan-device.hpp"
#include "vulkan-render-context.hpp"
#include "vulkan-command-pool.hpp"
#include "vulkan-asset-manager.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/log.hpp"
#include "../../core/sdl-window.hpp"
#include <set>
#include <vector>

using questart::VulkanContext;

namespace
{
    std::vector<std::string> getDesiredValidationLayers()
    {
        std::vector<std::string> result;

#ifndef NDEBUG
        // If we are in a debug build we will cultivate a list of validation layers.
        static const std::string logTag{"questart::VulkanContext::getDesiredValidationLayers"};

        // Collate which validations layers we are interested in applying if they are available.
        std::set<std::string> desiredLayers{"VK_LAYER_LUNARG_standard_validation"};

        // Iterate all the available layers for the current device.
        for (auto const& properties : vk::enumerateInstanceLayerProperties())
        {
            std::string layerName{properties.layerName};

            questart::log(logTag, "Available layer: " + layerName);

            // If we are interested in this layer, add it to the result list.
            if (desiredLayers.count(layerName))
            {
                questart::log(logTag, "*** Found desired layer: " + layerName);
                result.push_back(layerName);
            }
        }
#endif

        return result;
    }

    vk::UniqueInstance createInstance()
    {
        vk::ApplicationInfo applicationInfo{
            "A Simple Triangle",      // Application name
            VK_MAKE_VERSION(1, 0, 0), // Application version
            "A Simple Triangle",      // Engine name
            VK_MAKE_VERSION(1, 0, 0), // Engine version
            VK_MAKE_VERSION(1, 0, 0)  // Vulkan API version
        };

        // Find out what the mandatory Vulkan extensions are on the current device,
        // by this stage we would have already determined that the extensions are
        // available via the 'questart::vulkan::isVulkanAvailable()' call in our main engine.
        std::vector<std::string> requiredExtensionNames{
            questart::vulkan::getRequiredVulkanExtensionNames()};

        // Pack the extension names into a data format consumable by Vulkan.
        std::vector<const char*> extensionNames;
        for (const auto& extension : requiredExtensionNames)
        {
            extensionNames.push_back(extension.c_str());
        }

        // Determine what validation layers can and should be activated.
        std::vector<std::string> desiredValidationLayers{::getDesiredValidationLayers()};

        // Pack the validation layers into a data format consumable by Vulkan.
        std::vector<const char*> validationLayers;
        for (const auto& layer : desiredValidationLayers)
        {
            validationLayers.push_back(layer.c_str());
        }

        // Define the info for creating our Vulkan instance.
        vk::InstanceCreateInfo instanceCreateInfo{
            vk::InstanceCreateFlags(),                      // Flags
            &applicationInfo,                               // Application info
            static_cast<uint32_t>(validationLayers.size()), // Enabled layer count
            validationLayers.data(),                        // Enabled layer names
            static_cast<uint32_t>(extensionNames.size()),   // Enabled extension count
            extensionNames.data()                           // Enabled extension names
        };

        // Build a new Vulkan instance from the configuration.
        return vk::createInstanceUnique(instanceCreateInfo);
    }
} // namespace

struct VulkanContext::Internal
{
    const vk::UniqueInstance instance;
    const questart::VulkanPhysicalDevice physicalDevice;
    const questart::SDLWindow window;
    const questart::VulkanSurface surface;
    const questart::VulkanDevice device;
    const questart::VulkanCommandPool commandPool;
    questart::VulkanRenderContext renderContext;
    questart::VulkanAssetManager assetManager;

    Internal() 
               : instance(::createInstance()),
                 physicalDevice(questart::VulkanPhysicalDevice(*instance)),
                 window(questart::SDLWindow(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
                 surface(questart::VulkanSurface(*instance, physicalDevice, window)),
                 device(questart::VulkanDevice(physicalDevice, surface)),
                 commandPool(questart::VulkanCommandPool(device)),
                 renderContext(questart::VulkanRenderContext(window, physicalDevice, device, surface, commandPool)),
                 assetManager(questart::VulkanAssetManager())
    {
        questart::log("questart::VulkanContext", "Initialized Vulkan context successfully.");
    }

    void loadAssetManifest(const questart::AssetManifest& assetManifest)
    {
        assetManager.loadAssetManifest(physicalDevice, device, renderContext, commandPool, assetManifest);
    }

    bool renderBegin()
    {
        if (!renderContext.renderBegin(device))
        {
            recreateRenderContext();
            return false;
        }
        return true;
    }

    void render(const questart::assets::Pipeline& pipeline,
                const std::vector<questart::StaticMeshInstance>& staticMeshInstances)
    {
        assetManager.getPipeline(pipeline).render(device,
                                                  renderContext.getActiveCommandBuffer(),
                                                  assetManager,
                                                  staticMeshInstances);
    }

    void renderEnd()
    {
        if (!renderContext.renderEnd(device))
        {
            recreateRenderContext();
        }
    }

    void recreateRenderContext()
    {
        device.getDevice().waitIdle();
        renderContext = renderContext.recreate(window, physicalDevice, device, surface, commandPool);
        assetManager.reloadContextualAssets(physicalDevice, device, renderContext);
    }
};

VulkanContext::VulkanContext() : internal(questart::make_internal_ptr<Internal>()) {}

void VulkanContext::loadAssetManifest(const questart::AssetManifest& assetManifest)
{
    internal->loadAssetManifest(assetManifest);
}

bool VulkanContext::renderBegin()
{
    return internal->renderBegin();
}

void VulkanContext::render(const questart::assets::Pipeline& pipeline,
                           const std::vector<questart::StaticMeshInstance>& staticMeshInstances)
{
    internal->render(pipeline, staticMeshInstances);
}

void VulkanContext::renderEnd()
{
    internal->renderEnd();
}