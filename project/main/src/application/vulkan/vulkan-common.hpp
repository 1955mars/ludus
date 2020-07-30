#pragma once

#include <string>
#include <vector>

namespace questart::vulkan
{
    std::vector<std::string> getRequiredVulkanExtensionNames();

    bool isVulkanAvailable();
} // namespace questart::vulkan