#include "opengl-renderer.hpp"

using questart::OpenGLRenderer;

struct OpenGLRenderer::Internal
{
    const std::shared_ptr<questart::OpenGLAssetManager> assetManager;

    Internal(std::shared_ptr<questart::OpenGLAssetManager> assetManager) : assetManager(assetManager) {}

    void render(
        const questart::assets::Pipeline& pipeline,
        const std::vector<questart::StaticMeshInstance>& staticMeshInstances)
    {
        assetManager->getPipeline(pipeline).render(*assetManager, staticMeshInstances);
    }
};

OpenGLRenderer::OpenGLRenderer(std::shared_ptr<questart::OpenGLAssetManager> assetManager)
    : internal(questart::make_internal_ptr<Internal>(assetManager)) {}

void OpenGLRenderer::render(
    const questart::assets::Pipeline& pipeline,
    const std::vector<questart::StaticMeshInstance>& staticMeshInstances)
{
    internal->render(pipeline, staticMeshInstances);
}