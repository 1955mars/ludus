#include "scene-main.hpp"
#include "perspective-camera.hpp"
#include "static-mesh-instance.hpp"
#include "sdl-wrapper.hpp"
#include "player.hpp"

using questart::SceneMain;
using questart::assets::Pipeline;
using questart::assets::StaticMesh;
using questart::assets::Texture;

namespace
{
    questart::PerspectiveCamera createCamera(const questart::WindowSize& size)
    {
        return questart::PerspectiveCamera(static_cast<float>(size.width), static_cast<float>(size.height));
    }
} // namespace

struct SceneMain::Internal
{
    questart::PerspectiveCamera camera;
    std::vector<questart::StaticMeshInstance> staticMeshes;
    questart::Player player;
    const uint8_t* keyboardState;

    Internal(const questart::WindowSize& size)
        : camera(::createCamera(size)),
          player(questart::Player(glm::vec3{0.0f, 0.0f, 2.0f})),
          keyboardState(SDL_GetKeyboardState(nullptr)) {}

    questart::AssetManifest getAssetManifest()
    {
        return questart::AssetManifest{
            {Pipeline::Default},
            {StaticMesh::Crate, StaticMesh::Torus},
            {Texture::Crate, Texture::RedCrossHatch}};
    }

    void prepare()
    {
        staticMeshes.push_back(questart::StaticMeshInstance{
            StaticMesh::Crate,           // Mesh
            Texture::Crate,              // Texture
            glm::vec3{0.4f, 0.6f, 0.0f}, // Position
            glm::vec3{0.6f, 0.6f, 0.6f}, // Scale
            glm::vec3{0.0f, 0.4f, 0.9f}, // Rotation axis
            0.0f});                      // Initial rotation

        staticMeshes.push_back(questart::StaticMeshInstance{
            StaticMesh::Torus,            // Mesh
            Texture::RedCrossHatch,       // Texture
            glm::vec3{-0.6f, 0.4f, 0.0f}, // Position
            glm::vec3{0.4f, 0.4f, 0.4f},  // Scale
            glm::vec3{0.2f, 1.0f, 0.4f},  // Rotation axis
            0.0f});                       // Initial rotation

        staticMeshes.push_back(questart::StaticMeshInstance{
            StaticMesh::Crate,             // Mesh
            Texture::Crate,                // Texture
            glm::vec3{-0.5f, -0.5f, 0.0f}, // Position
            glm::vec3{0.7f, 0.3f, 0.3f},   // Scale
            glm::vec3{0.2f, 0.6f, 0.1f},   // Rotation axis
            90.0f});                       // Initial rotation

        staticMeshes.push_back(questart::StaticMeshInstance{
            StaticMesh::Torus,            // Mesh
            Texture::RedCrossHatch,       // Texture
            glm::vec3{0.6f, -0.4f, 0.0f}, // Position
            glm::vec3{0.4f, 0.4f, 0.4f},  // Scale
            glm::vec3{0.6f, 0.3f, 0.1f},  // Rotation axis
            50.0f});

    }

    void update(const float& delta)
    {
        processInput(delta);

        camera.configure(player.getPosition(), player.getDirection());

        //const glm::mat4 cameraMatrix{camera.getProjectionMatrix() * camera.getViewMatrix()};

        std::vector<glm::mat4> viewMatrix{ camera.getViewMatrix() };
        std::vector<glm::mat4> projectionMatrix{ camera.getProjectionMatrix() };

        for (auto& staticMesh : staticMeshes)
        {
            staticMesh.rotateBy(delta * 45.0f);
            staticMesh.update(viewMatrix, projectionMatrix);
        }
    }

    void render(questart::Renderer& renderer)
    {
        renderer.render(Pipeline::Default, staticMeshes);
    }

    void onWindowResized(const questart::WindowSize& size)
    {
        camera = ::createCamera(size);
    }

    void processInput(const float& delta)
    {
        if (keyboardState[SDL_SCANCODE_UP])
        {
            player.moveForward(delta);
        }

        if (keyboardState[SDL_SCANCODE_DOWN])
        {
            player.moveBackward(delta);
        }

        if (keyboardState[SDL_SCANCODE_A])
        {
            player.moveUp(delta);
        }

        if (keyboardState[SDL_SCANCODE_Z])
        {
            player.moveDown(delta);
        }

        if (keyboardState[SDL_SCANCODE_LEFT])
        {
            player.turnLeft(delta);
        }

        if (keyboardState[SDL_SCANCODE_RIGHT])
        {
            player.turnRight(delta);
        }
    }
};

SceneMain::SceneMain(const questart::WindowSize& size)
    : internal(questart::make_internal_ptr<Internal>(size)) {}

questart::AssetManifest SceneMain::getAssetManifest()
{
    return internal->getAssetManifest();
}


void SceneMain::prepare()
{
    internal->prepare();
}

void SceneMain::update(const float& delta, void* tracking)
{
    internal->update(delta);
}

void SceneMain::render(questart::Renderer& renderer)
{
    internal->render(renderer);
}

void SceneMain::onWindowResized(const questart::WindowSize& size)
{
    internal->onWindowResized(size);
}