#include "../core/sdl-wrapper.hpp"
#include "application.hpp"

using questart::Application;

void Application::startApplication()
{
    while (runMainLoop())
    {
        // Just waiting for the main loop to end
    }
}

bool Application::runMainLoop()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                return false;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return false;
                }
                break;

            default:
                break;
        }
    }

    // Perform our rendering for this frame
    render();

    return true;
}