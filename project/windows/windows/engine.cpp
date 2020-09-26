#include "engine.hpp"

#include <SDL_image.h>
#include <stdexcept>
#include <string>

using questart::Engine;

struct Engine::Internal
{
    const std::string classLogTag;
    const float performanceFrequency;
    uint64_t currentTime;
    uint64_t previousTime;
    std::unique_ptr<questart::Application> app;

    Internal() : classLogTag("questart::Engine::"),
        performanceFrequency(static_cast<float>(SDL_GetPerformanceFrequency())),
        currentTime(SDL_GetPerformanceCounter()),
        previousTime(currentTime) {}

    float timeStep()
    {
        previousTime = currentTime;
        currentTime = SDL_GetPerformanceCounter();

        float elapsed{ (currentTime - previousTime) * 1000.0f };
        return (elapsed / performanceFrequency) * 0.001f;
    }
  

    void run()
    {
        static const std::string logTag{classLogTag + "run"};

        questart::log(logTag, "Starting engine ...");
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        questart::log(logTag, "SDL2 initialized successfully!");

        if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
        {
            throw std::runtime_error(logTag + " Could not initialize SDL2_image ...");
        }
        questart::log(logTag, "SDL2_image initialized successfully with PNG support ...");

        app = resolveApplication();

        while (runMainLoop())
        {
            app->update(timeStep());
            app->render();

        }
    }

    std::unique_ptr<questart::Application> resolveApplication()
    {
        static const std::string logTag{classLogTag + "resolveApplication"};

        if (questart::vulkan::isVulkanAvailable())
        {
            try
            {
                questart::log(logTag, "Creating Vulkan application ...");
                return std::make_unique<questart::VulkanApplication>();
            }
            catch (const std::exception& error)
            {
                questart::log(logTag, "Vulkan application failed to initialize.", error);
            }
        }

        try
        {
            questart::log(logTag, "Creating OpenGL Application ...");
            return std::make_unique<questart::OpenGLApplication>();
        }
        catch (const std::exception& error)
        {
            questart::log(logTag, "OpenGL application failed to initialize.", error);
        }

        throw std::runtime_error(logTag + "No application can run in the current environment!");
    }

    bool runMainLoop()
    {
        SDL_Event event;

        //questart::log("runMainLoop", " running!");

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    app->onWindowResized();
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
        //app->update(timeStep());

        // Perform our rendering for this frame
        //app->render();

        return true;
    }

    ~Internal()
    {
        SDL_Quit();
    }
};

Engine::Engine() : internal(questart::make_internal_ptr<Internal>()) {}

void Engine::run()
{
    internal->run();
}