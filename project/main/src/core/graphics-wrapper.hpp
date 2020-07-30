#pragma once

#if defined WIN32
#define GLEW_STATIC
#include <GL/glew.h>
#include <vulkan/vulkan.hpp>
#elif __ANDROID__
#include <vulkan_wrapper.h>
#undef VK_NO_PROTOTYPES
#include <vulkan/vulkan.hpp>
#include <GLES3/gl3.h>
#define USING_GLES
#endif