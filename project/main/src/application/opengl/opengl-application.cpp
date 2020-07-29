#include "opengl-application.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/log.hpp"
#include "../../core/sdl-wrapper.hpp"
#include "../../scene/scene-main.hpp"
#include "opengl-asset-manager.hpp"
#include "opengl-renderer.hpp"

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

    std::shared_ptr<questart::OpenGLAssetManager> createAssetManager()
    {
        return std::make_shared<questart::OpenGLAssetManager>(questart::OpenGLAssetManager());
    }

    questart::OpenGLRenderer createRenderer(std::shared_ptr<questart::OpenGLAssetManager> assetManager)
    {
        return questart::OpenGLRenderer(assetManager);
    }

    std::unique_ptr<questart::Scene> createMainScene(questart::AssetManager& assetManager)
    {
        std::pair<uint32_t, uint32_t> displaySize{questart::sdl::getDisplaySize()};
        std::unique_ptr<questart::Scene> scene{std::make_unique<questart::SceneMain>(
            static_cast<float>(displaySize.first),
            static_cast<float>(displaySize.second))};
        scene->prepare(assetManager);
        return scene;
    }
}// namespace

struct OpenGLApplication::Internal
{
    SDL_Window* window;
    SDL_GLContext context;
    const std::shared_ptr<questart::OpenGLAssetManager> assetManager;
    questart::OpenGLRenderer renderer;
    std::unique_ptr<questart::Scene> scene;

    Internal() : window(questart::sdl::createWindow(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE)),
                 context(::createContext(window)),
                 assetManager(::createAssetManager()),
                 renderer(::createRenderer(assetManager)) {}

    void update(const float& delta)
    {
        getScene().update(delta);
    }

    questart::Scene& getScene()
    {
        if (!scene)
        {
            scene = ::createMainScene(*assetManager);
        }

        return *scene;
    }

    void render()
    {
        SDL_GL_MakeCurrent(window, context);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        getScene().render(renderer);

        SDL_GL_SwapWindow(window);
    }

    ~Internal()
    {
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
    }

};

OpenGLApplication::OpenGLApplication() : internal(questart::make_internal_ptr<Internal>()) {}

void OpenGLApplication::update(const float& delta)
{
    internal->update(delta);
}

void OpenGLApplication::render()
{
    internal->render();
}