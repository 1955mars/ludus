#include "sdl-wrapper.hpp"
#include "platform.hpp"

namespace
{
	bool shouldDisplayFullScreen()
	{
		switch (questart::getCurrentPlatform())
		{
			case questart::Platform::android:
				return true;
			default:
				return false;
		}
	}
}

questart::WindowSize questart::sdl::getWindowSize(SDL_Window* window)
{
    int width{0};
    int height{0};
    SDL_GetWindowSize(window, &width, &height);
    return questart::WindowSize{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}


questart::WindowSize questart::sdl::getInitialWindowSize()
{
    const questart::Platform platform{questart::getCurrentPlatform()};

    if (platform == questart::Platform::android)
    {
        // For mobile platforms we will fetch the full screen size.
        SDL_DisplayMode displayMode;
        SDL_GetDesktopDisplayMode(0, &displayMode);
        return questart::WindowSize{static_cast<uint32_t>(displayMode.w), static_cast<uint32_t>(displayMode.h)};
    }

    // For other platforms we'll just show a fixed size window.
    return questart::WindowSize{640, 480};
}

SDL_Window* questart::sdl::createWindow(const uint32_t& windowFlags)
{
    questart::WindowSize windowSize{questart::sdl::getInitialWindowSize()};

    SDL_Window* window{SDL_CreateWindow(
        "A Simple Triangle",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        windowSize.width, windowSize.height,
        windowFlags)};

    if (::shouldDisplayFullScreen())
    {
        SDL_SetWindowFullscreen(window, SDL_TRUE);
    }

    return window;
}