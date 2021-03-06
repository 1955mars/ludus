#include "vulkan-device.hpp"
#include <vector>
#include <sstream>
#include <log.hpp>
#include <set>

#if defined(QUEST)
#include "ovr-wrapper.hpp"
#include "engine.hpp"
#endif

using questart::VulkanDevice;

namespace
{
    struct QueueConfig
    {
        uint32_t graphicsQueueIndex;
        uint32_t presentationQueueIndex;
        bool hasDiscretePresentationQueue;
    };

    QueueConfig getQueueConfig(const vk::PhysicalDevice& physicalDevice)
    {
        static const std::string logTag{"questart::VulkanPhysicalDevice::getGraphicsQueueFamilyIndex"};

        // Assign properties to track selection of graphics and presentation queues,
        // initially with an unset marker value.
        constexpr uint32_t unsetQueueIndex{std::numeric_limits<uint32_t>::max()};
        uint32_t graphicsQueueIndex{unsetQueueIndex};
        uint32_t presentationQueueIndex{unsetQueueIndex};

        // Fetch all the queue family properties supported by the physical device.
        std::vector<vk::QueueFamilyProperties> queueFamilies{physicalDevice.getQueueFamilyProperties()};

        // Search through the available queue families, looking for one that supports graphics.
        for (size_t i = 0; i < queueFamilies.size(); i++)
        {
            vk::QueueFamilyProperties properties{queueFamilies[i]};

            // If the current queue family has graphics capability we will evaluate it as a
            // candidate for both the graphics queue and the presentation queue.
            if (properties.queueCount > 0 && properties.queueFlags & vk::QueueFlagBits::eGraphics)
            {
                const uint32_t currentQueueIndex{static_cast<uint32_t>(i)};

                // If we haven't yet chosen an index for the graphics queue (because it is unset),
                // then remember this index as the graphics queue index.
                if (graphicsQueueIndex == unsetQueueIndex)
                {
                    graphicsQueueIndex = currentQueueIndex;
                    presentationQueueIndex = currentQueueIndex;
                    break;
                }

                // We now need to see if the queue index can also behave as a presentation queue and
                // if so, both the graphics and presentation queue indices will be the same, effectively
                // meaning that we will only need to create a single queue and use it for both purposes.
            /*    if (physicalDevice.getSurfaceSupportKHR(currentQueueIndex, surface))
                {
                    graphicsQueueIndex = currentQueueIndex;
                    presentationQueueIndex = currentQueueIndex;

                    // Since we have now discovered a queue index that can do BOTH graphics and presentation
                    // we won't bother looking any further in the loop.
                    break;
                }*/
            }
        }

        // If we couldn't find any queues that can perform graphics operations we are out.
        if (graphicsQueueIndex == unsetQueueIndex)
        {
            throw std::runtime_error(logTag + ": Could not find a graphics queue.");
        }

        // If we found a graphics queue, but not one that could also do presentation then we
        // need to find a discrete presentation queue to use instead, meaning we will end
        // up with two queues to manage instead of one that can do both.
    /*    if (presentationQueueIndex == unsetQueueIndex)
        {
            for (size_t i = 0; i < queueFamilies.size(); i++)
            {
                const uint32_t currentQueueIndex{static_cast<uint32_t>(i)};

                if (physicalDevice.getSurfaceSupportKHR(currentQueueIndex, surface))
                {
                    presentationQueueIndex = currentQueueIndex;
                    break;
                }
            }
        }*/

        // Once more we will see if we have a presentation queue or not - this time if
        // we still do not have one then we are out again.
        if (presentationQueueIndex == unsetQueueIndex)
        {
            throw std::runtime_error(logTag + ": Could not find a presentation queue.");
        }

        // At this point we have valid graphics queue and presentation queue indices
        // so we will wrap them up in a result object and return it. Note that we
        // will also record whether the presentation queue is 'discrete' meaning that
        // it is a different queue to the graphics queue.
        return QueueConfig{
            graphicsQueueIndex,
            presentationQueueIndex,
            graphicsQueueIndex != presentationQueueIndex};
    }

#if defined(QUEST)
    std::vector<std::string> getVrApiVulkanDeviceExtensions()
    {
        //vrapi device extensions
        char deviceExtensionNames[4096];
        uint32_t deviceExtensionNameSize = sizeof(deviceExtensionNames);

        if(vrapi_GetDeviceExtensionsVulkan(deviceExtensionNames, &deviceExtensionNameSize))
        {
            throw std::runtime_error("Failed to get vrapi device extensions");
        }

        std::string extNames = deviceExtensionNames;

        std::istringstream sNames (extNames);

        std::vector<std::string> result;

        while(sNames) {
            std::string name;
            sNames >> name;
            result.push_back(name);
        };

        result.pop_back();
        result.pop_back();

        result.push_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);
        result.push_back(VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME);
        result.push_back(VK_KHR_EXTERNAL_MEMORY_FD_EXTENSION_NAME);

        return result;
    }
#endif

    std::vector<std::string> getDesiredValidationLayers()
    {
        std::vector<std::string> result;

#ifndef NDEBUG
        // If we are in a debug build we will cultivate a list of validation layers.
        static const std::string logTag{"questart::VulkanContext::getDesiredValidationLayers"};

#if defined(QUEST)
        std::set<std::string> desiredLayers{"VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_GOOGLE_threading", "VK_LAYER_GOOGLE_unique_objects"};
#else
        // Collate which validations layers we are interested in applying if they are available.
        std::set<std::string> desiredLayers{"VK_LAYER_LUNARG_standard_validation"};
#endif

        // Iterate all the available layers for the current device.
        for (auto const& properties : vk::enumerateInstanceLayerProperties())
        {
            std::string layerName = properties.layerName;

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

    vk::UniqueDevice createDevice(const questart::VulkanPhysicalDevice& physicalDevice,
                                  const QueueConfig& queueConfig)
    {
        const float deviceQueuePriority{1.0f};

        // Hold a list of queue creation objects to use in our logical device,
        // initialising it with the graphics queue configuration.
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos{
            vk::DeviceQueueCreateInfo{
                vk::DeviceQueueCreateFlags(),   // Flags
                queueConfig.graphicsQueueIndex, // Queue family index
                1,                              // Queue count
                &deviceQueuePriority            // Queue priority
            }};

        // Add a presentation queue if the presentation queue needs to be
        // discrete from the graphics queue.
        if (queueConfig.hasDiscretePresentationQueue)
        {
            queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
                vk::DeviceQueueCreateFlags(),       // Flags
                queueConfig.presentationQueueIndex, // Queue family index
                1,                                  // Queue count
                &deviceQueuePriority                // Queue priority
            });
        }

#if defined(QUEST)
        std::vector<std::string> requiredVrApiDeviceExtensions{::getVrApiVulkanDeviceExtensions()};
        std::vector<const char*> extensionNames;
        for(const auto& extension : requiredVrApiDeviceExtensions)
        {
            questart::log("Create Device: ", "VrApi Device Extension: " + extension);
            extensionNames.push_back(extension.c_str());
        }
#else

        // We also need to request the swapchain extension be activated as we will need to use a swapchain
        std::vector<const char*> extensionNames{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#endif

        // Specify which physical device features to expose in our logical device
        vk::PhysicalDeviceFeatures physicalDeviceFeatures;

        // If shader based multisampling is available we will activate it.
        if (physicalDevice.isShaderMultiSamplingSupported())
        {
            physicalDeviceFeatures.sampleRateShading = true;
        }

        // If anisotropic filtering is available we will activate it.
        if (physicalDevice.isAnisotropicFilteringSupported())
        {
            physicalDeviceFeatures.samplerAnisotropy = true;
        }

        std::vector<std::string> desiredValidationLayers{::getDesiredValidationLayers()};
        std::vector<const char*> validationLayers;
        for(const auto& layer :desiredValidationLayers)
        {
            questart::log("questart::CreateInstance::ValidationLayer: ", layer);
            validationLayers.push_back(layer.c_str());
        }

        // Take the queue and extension name configurations and form the device creation definition.
        vk::DeviceCreateInfo deviceCreateInfo{
            vk::DeviceCreateFlags(),                        // Flags
            static_cast<uint32_t>(queueCreateInfos.size()), // Queue create info list count
            queueCreateInfos.data(),                        // Queue create info list
            static_cast<uint32_t>(desiredValidationLayers.size()),                                              // Enabled layer count
            validationLayers.data(),                                        // Enabled layer names
            static_cast<uint32_t>(extensionNames.size()),   // Enabled extension count
            extensionNames.data(),                          // Enabled extension names
            &physicalDeviceFeatures                         // Physical device features
        };

        // Create a logical device with all the configuration we collated.
        return physicalDevice.getPhysicalDevice().createDeviceUnique(deviceCreateInfo);
    }

    vk::Queue getQueue(const vk::Device& device, const uint32_t& queueIndex)
    {
        return device.getQueue(queueIndex, 0);
    }

    std::vector<vk::UniqueSemaphore> createSemaphores(const vk::Device& device,
                                                      const uint32_t& count)
    {
        std::vector<vk::UniqueSemaphore> semaphores;
        vk::SemaphoreCreateInfo info;

        for (uint32_t i = 0; i < count; i++)
        {
            semaphores.push_back(device.createSemaphoreUnique(info));
        }

        return semaphores;
    }

    std::vector<vk::UniqueFence> createFences(const vk::Device& device, const uint32_t& count)
    {
        std::vector<vk::UniqueFence> fences;
        vk::FenceCreateInfo info{vk::FenceCreateFlagBits::eSignaled};

        for (uint32_t i = 0; i < count; i++)
        {
            fences.push_back(device.createFenceUnique(info));
        }

        return fences;
    }

    
    vk::UniqueShaderModule createShaderModule(const vk::Device& device, const std::vector<char>& shaderCode)
    {
        vk::ShaderModuleCreateInfo info{
            vk::ShaderModuleCreateFlags(),                         // Flags
            shaderCode.size(),                                     // Code size
            reinterpret_cast<const uint32_t*>(shaderCode.data())}; // Code

        return device.createShaderModuleUnique(info);
    }

} // namespace

struct VulkanDevice::Internal
{
    const QueueConfig queueConfig;
    const vk::UniqueDevice device;
    const vk::Queue graphicsQueue;
    const vk::Queue presentationQueue;

    Internal(const questart::VulkanPhysicalDevice& physicalDevice)
        : queueConfig(::getQueueConfig(physicalDevice.getPhysicalDevice())),
          device(::createDevice(physicalDevice, queueConfig)),
          graphicsQueue(::getQueue(device.get(), queueConfig.graphicsQueueIndex)),
          presentationQueue(::getQueue(device.get(), queueConfig.presentationQueueIndex))
    {
#if defined(QUEST)
        questart::Engine::graphicsQueue = graphicsQueue;
#endif
    }

    ~Internal()
    {
        // We need to wait for the device to become idle before allowing it to be destroyed.
        device->waitIdle();
    }
};

VulkanDevice::VulkanDevice(const questart::VulkanPhysicalDevice& physicalDevice)
    : internal(questart::make_internal_ptr<Internal>(physicalDevice)) {}

const vk::Device& VulkanDevice::getDevice() const
{
    return *internal->device;
}

uint32_t VulkanDevice::getGraphicsQueueIndex() const
{
    return internal->queueConfig.graphicsQueueIndex;
}

uint32_t VulkanDevice::getPresentationQueueIndex() const
{
    return internal->queueConfig.presentationQueueIndex;
}

bool VulkanDevice::hasDiscretePresentationQueue() const
{
    return internal->queueConfig.hasDiscretePresentationQueue;
}

const vk::Queue& VulkanDevice::getGraphicsQueue() const
{
    return internal->graphicsQueue;
}

const vk::Queue& VulkanDevice::getPresentationQueue() const
{
    return internal->presentationQueue;
}

std::vector<vk::UniqueSemaphore> VulkanDevice::createSemaphores(const uint32_t& count) const
{
    return ::createSemaphores(internal->device.get(), count);
}

std::vector<vk::UniqueFence> VulkanDevice::createFences(const uint32_t& count) const
{
    return ::createFences(internal->device.get(), count);
}

vk::UniqueShaderModule VulkanDevice::createShaderModule(const std::vector<char>& shaderCode) const
{
    return ::createShaderModule(internal->device.get(), shaderCode);
}