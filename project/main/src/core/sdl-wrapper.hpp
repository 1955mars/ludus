#pragma once

#include <SDL.h>
#include <SDL_vulkan.h>
#include <utility>
#include "window-size.hpp"

namespace questart::sdl
{
    questart::WindowSize getInitialWindowSize();

    questart::WindowSize getWindowSize(SDL_Window* window);

	SDL_Window* createWindow(const uint32_t& windowFlags);
}