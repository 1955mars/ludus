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

std::pair<uint32_t, uint32_t> questart::sdl::getDisplaySize()
{
	uint32_t displayWidth{ 0 };
	uint32_t displayHeight{ 0 };

	switch (questart::getCurrentPlatform())
	{
		default:
		{
			displayWidth = 640;
			displayHeight = 480;
			break;
		}
	}
	return std::make_pair(displayWidth, displayHeight);
}

SDL_Window* questart::sdl::createWindow(const uint32_t& windowFlags)
{
	std::pair<uint32_t, uint32_t> displaySize{ questart::sdl::getDisplaySize() };

	SDL_Window* window{ SDL_CreateWindow(
		"A Simple Triangle",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		displaySize.first, displaySize.second,
		windowFlags) };

	if (::shouldDisplayFullScreen())
	{
		SDL_SetWindowFullscreen(window, SDL_TRUE);
	}

	return window;
}