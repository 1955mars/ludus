#pragma once

#include "../../core/asset-manager.hpp"
#include "../../core/internal_ptr.hpp"
#include "opengl-mesh.hpp"
#include "opengl-pipeline.hpp"
#include "opengl-texture.hpp"

namespace questart
{
    struct OpenGLAssetManager : public questart::AssetManager
    {
        OpenGLAssetManager();

        void loadPipelines(const std::vector<questart::assets::Pipeline>& pipelines) override;

        void loadStaticMeshes(const std::vector<questart::assets::StaticMesh>& staticMeshes) override;

        void loadTextures(const std::vector<questart::assets::Texture>& textures) override;

        const questart::OpenGLPipeline& getPipeline(const questart::assets::Pipeline& pipeline) const;

        const questart::OpenGLMesh& getStaticMesh(const questart::assets::StaticMesh& staticMesh) const;

        const questart::OpenGLTexture& getTexture(const questart::assets::Texture& texture) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart