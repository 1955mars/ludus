#include "renderer-api.hpp"

#include "../graphics/vulkan/vulkan-common.hpp"

namespace questart
{
    RendererAPI::API RendererAPI::sAPI = questart::vulkan::isVulkanAvailable() ? RendererAPI::API::Vulkan : RendererAPI::API::OpenGL;
}