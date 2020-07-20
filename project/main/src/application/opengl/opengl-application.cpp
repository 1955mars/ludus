#include "opengl-application.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/sdl-wrapper.hpp"
#include "../../core/log.hpp"
#include "../../core/assets.hpp"
#include <string>

using questart::OpenGLApplication;

namespace
{
    SDL_GLContext createContext(SDL_Window* window)
    {
        static const std::string logTag{"questart::OpenGLApplication::createContext"};

        SDL_GLContext context{SDL_GL_CreateContext(window)};

#ifdef WIN32
        glewInit();
#endif //  WIN32

        int viewPortWidth;
        int viewPortHeight;
        SDL_GL_GetDrawableSize(window, &viewPortWidth, &viewPortHeight);
        questart::log(logTag, "Created OpenGL context with viewport size: " + std::to_string(viewPortWidth) + " x " + std::to_string(viewPortHeight));

        glClearDepthf(1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glEnable(GL_CULL_FACE);
        glViewport(0, 0, viewPortWidth, viewPortHeight);

        return context;
    }
}// namespace

struct OpenGLApplication::Internal
{
    SDL_Window* window;
    SDL_GLContext context;
    const questart::Mesh mesh;

    Internal() : window(questart::sdl::createWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI)),
                 context(::createContext(window)),
                     mesh(questart::assets::loadOBJFile("assets/models/crate.obj")) 
    {
        questart::log("CRATE!", "Crate has " + std::to_string(mesh.getVertices().size()) +
            " vertices and " + std::to_string(mesh.getIndices().size()) + " indices.");
    }

    void render()
    {
        SDL_GL_MakeCurrent(window, context);

        glClearColor(0.3f, 0.7f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        SDL_GL_SwapWindow(window);
    }

    ~Internal()
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
    }

};

OpenGLApplication::OpenGLApplication() : internal(questart::make_internal_ptr<Internal>()) {}

void OpenGLApplication::render()
{
    internal->render();
}