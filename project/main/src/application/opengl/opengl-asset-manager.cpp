#include "opengl-asset-manager.hpp"
#include "../../core/assets.hpp"
#include <unordered_map>

using questart::OpenGLAssetManager;

struct OpenGLAssetManager::Internal
{
    std::unordered_map<questart::assets::Pipeline, questart::OpenGLPipeline> pipelineCache;
    std::unordered_map<questart::assets::StaticMesh, questart::OpenGLMesh> staticMeshCache;
    std::unordered_map<questart::assets::Texture, questart::OpenGLTexture> textureCache;

    Internal() {}

    void loadPipelines(const std::vector<questart::assets::Pipeline>& pipelines)
    {
        for (const auto& pipeline : pipelines)
        {
            if (pipelineCache.count(pipeline) == 0)
            {
                pipelineCache.insert(std::make_pair(
                    pipeline,
                    questart::OpenGLPipeline(questart::assets::resolvePipelinePath(pipeline))));
            }
        }
    }

    void loadStaticMeshes(const std::vector<questart::assets::StaticMesh>& staticMeshes)
    {
        for (const auto& staticMesh : staticMeshes)
        {
            if (staticMeshCache.count(staticMesh) == 0)
            {
                staticMeshCache.insert(std::make_pair(
                    staticMesh,
                    questart::OpenGLMesh(questart::assets::loadOBJFile(questart::assets::resolveStaticMeshPath(staticMesh)))));
            }
        }
    }

    void loadTextures(const std::vector<questart::assets::Texture>& textures)
    {
        for (const auto& texture : textures)
        {
            if (textureCache.count(texture) == 0)
            {
                textureCache.insert(std::pair(
                    texture,
                    questart::OpenGLTexture(questart::assets::loadBitmap(questart::assets::resolveTexturePath(texture)))));
            }
        }
    }
};

OpenGLAssetManager::OpenGLAssetManager() : internal(questart::make_internal_ptr<Internal>()) {}

void OpenGLAssetManager::loadPipelines(const std::vector<questart::assets::Pipeline>& pipelines)
{
    internal->loadPipelines(pipelines);
}

void OpenGLAssetManager::loadStaticMeshes(const std::vector<questart::assets::StaticMesh>& staticMeshes)
{
    internal->loadStaticMeshes(staticMeshes);
}

void OpenGLAssetManager::loadTextures(const std::vector<questart::assets::Texture>& textures)
{
    internal->loadTextures(textures);
}

const questart::OpenGLPipeline& OpenGLAssetManager::getPipeline(const questart::assets::Pipeline& pipeline) const
{
    return internal->pipelineCache.at(pipeline);
}

const questart::OpenGLMesh& OpenGLAssetManager::getStaticMesh(const questart::assets::StaticMesh& staticMesh) const
{
    return internal->staticMeshCache.at(staticMesh);
}

const questart::OpenGLTexture& OpenGLAssetManager::getTexture(const questart::assets::Texture& texture) const
{
    return internal->textureCache.at(texture);
}