#include "../core/sdl-wrapper.hpp"
#include "application.hpp"

using questart::Application;

struct Application::Internal
{
    const float performanceFrequency;
    uint64_t currentTime;
    uint64_t previousTime;

    Internal() : performanceFrequency(static_cast<float>(SDL_GetPerformanceFrequency())),
                 currentTime(SDL_GetPerformanceCounter()),
                 previousTime(currentTime) {}

    float timeStep()
    {
        previousTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();

        float elapsed{(currentTime - previousTime) * 1000.0f};
        return (elapsed / performanceFrequency) * 0.001f;
    }
};


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
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    onWindowResized();
                }
                break;

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

    // Perform our updating for this frame
    update(internal->timeStep());

    // Perform our rendering for this frame
    render();

    return true;
}

Application::Application() : internal(questart::make_internal_ptr<Internal>()) {}