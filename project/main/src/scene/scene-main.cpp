#include "scene-main.hpp"
#include "../core/perspective-camera.hpp"
#include "../core/static-mesh-instance.hpp"

using questart::SceneMain;
using questart::assets::Pipeline;
using questart::assets::StaticMesh;
using questart::assets::Texture;

namespace
{
    questart::PerspectiveCamera createCamera(const float& width, const float& height)
    {
        return questart::PerspectiveCamera(width, height);
    }
} // namespace

struct SceneMain::Internal
{
    questart::PerspectiveCamera camera;
    std::vector<questart::StaticMeshInstance> staticMeshes;

    Internal(const float& screenWidth, const float& screenHeight)
        : camera(::createCamera(screenWidth, screenHeight)) {}

    void prepare(questart::AssetManager& assetManager)
    {
        assetManager.loadPipelines({Pipeline::Default});
        assetManager.loadStaticMeshes({StaticMesh::Crate, StaticMesh::Torus});
        assetManager.loadTextures({Texture::Crate, Texture::RedCrossHatch});

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
        const glm::mat4 cameraMatrix{camera.getProjectionMatrix() * camera.getViewMatrix()};

        for (auto& staticMesh : staticMeshes)
        {
            staticMesh.rotateBy(delta * 45.0f);
            staticMesh.update(cameraMatrix);
        }
    }

    void render(questart::Renderer& renderer)
    {
        renderer.render(Pipeline::Default, staticMeshes);
    }
};

SceneMain::SceneMain(const float& screenWidth, const float& screenHeight)
    : internal(questart::make_internal_ptr<Internal>(screenWidth, screenHeight)) {}

void SceneMain::prepare(questart::AssetManager& assetManager)
{
    internal->prepare(assetManager);
}

void SceneMain::update(const float& delta)
{
    internal->update(delta);
}

void SceneMain::render(questart::Renderer& renderer)
{
    internal->render(renderer);
}