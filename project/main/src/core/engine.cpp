#include "engine.hpp"
#include "../application/application.hpp"
#include "../application/opengl/opengl-application.hpp"
#include "log.hpp"
#include "sdl-wrapper.hpp"
#include <stdexcept>
#include <string>

using questart::Engine;

struct Engine::Internal
{
    const std::string classLogTag;

    Internal() : classLogTag("questart::Engine::") {}

    void run()
    {
        static const std::string logTag{classLogTag + "run"};

        questart::log(logTag, "Starting engine ...");
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        questart::log(logTag, "SDL2 initialized successfully!");
        resolveApplication()->startApplication();
    }

    std::unique_ptr<questart::Application> resolveApplication()
    {
        static const std::string logTag{classLogTag + "resolveApplication"};

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