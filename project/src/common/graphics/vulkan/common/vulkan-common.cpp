#include "vulkan-common.hpp"
#include "../../../core/graphics-wrapper.hpp"
#include "../../../core/log.hpp"

#if defined(QUEST)
#include "ovr-wrapper.hpp"
#else
#include "sdl-wrapper.hpp"
#endif


#include <set>
#include <sstream>

std::vector<std::string> questart::vulkan::getRequiredVulkanExtensionNames()
{
#if defined(QUEST)
    char instanceExtensionNames[4096];
    uint32_t instanceExtensionNamesSize = sizeof(instanceExtensionNames);

    if (vrapi_GetInstanceExtensionsVulkan(instanceExtensionNames, &instanceExtensionNamesSize))
    {
        throw std::runtime_error("Failed to get vulkan vrapi instance extensions");
    }

    std::string extNames = instanceExtensionNames;

    std::istringstream sNames (extNames);

    std::vector<std::string> result;

    while(sNames) {
        std::string name;
        sNames >> name;
        result.push_back(name);
    };

    result.pop_back();
#ifndef NDEBUG
    result.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
#endif
    //result.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    return result;
#else
    uint32_t extensionCount;

    SDL_Vulkan_GetInstanceExtensions(nullptr, &extensionCount, nullptr);

    auto extensionNames{std::make_unique<const char**>(new const char*[extensionCount])};
    SDL_Vulkan_GetInstanceExtensions(nullptr, &extensionCount, *extensionNames);
    std::vector<std::string> result(*extensionNames, *extensionNames + extensionCount);

    return result;
#endif
}

bool questart::vulkan::isVulkanAvailable()
{
    static const std::string logTag{"questart::vulkan::isVulkanAvailable"};

#ifdef __ANDROID__
    // Try to dynamically load the Vulkan library and seed the function pointer mapping.
    if (!InitVulkan())
    {
        return false;
    }
#endif

    
    // Check if SDL itself can load Vulkan.
    if (SDL_Vulkan_LoadLibrary(nullptr) != 0)
    {
        questart::log(logTag, "No SDL Vulkan support found.");
        return false;
    }

    // Determine what Vulkan extensions are required by SDL to be able to run Vulkan then pump
    // them into a 'set' so we can evaluate them easily.
    std::vector<std::string> requiredExtensionNamesSource{
        questart::vulkan::getRequiredVulkanExtensionNames()};

    std::set<std::string> requiredExtensionNames(
        requiredExtensionNamesSource.begin(),
        requiredExtensionNamesSource.end());

    // There should always be required extensions so we should never get 0.
    if (requiredExtensionNames.empty())
    {
        questart::log(logTag, "No Vulkan required extensions found.");
        return false;
    }

    // Iterate all the available Vulkan extensions on the current device, draining
    // each one from the required extensions set along the way.
    for (const vk::ExtensionProperties& availableExtension : vk::enumerateInstanceExtensionProperties())
    {
        requiredExtensionNames.erase(std::string(availableExtension.extensionName));
    }

    // If our required extensions set isn't empty it means that one or more of
    // them were not found in the available extensions, so we don't have what
    // we require to successfully create a Vulkan instance.
    if (!requiredExtensionNames.empty())
    {
        questart::log(logTag, "Missing one or more required Vulkan extensions.");
        return false;
    }

    questart::log(logTag, "Vulkan is available.");
    return true;
}