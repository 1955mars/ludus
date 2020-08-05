#include "scene-main.hpp"
#include "../core/perspective-camera.hpp"
#include "../core/static-mesh-instance.hpp"

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

    Internal(const questart::WindowSize& size)
        : camera(::createCamera(size)) {}

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

    void onWindowResized(const questart::WindowSize& size)
    {
        camera = ::createCamera(size);
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

void SceneMain::update(const float& delta)
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