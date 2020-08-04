#pragma once

#include "../../core/asset-manifest.hpp"
#include "../../core/internal_ptr.hpp"
#include "opengl-mesh.hpp"
#include "opengl-pipeline.hpp"
#include "opengl-texture.hpp"

namespace questart
{
    struct OpenGLAssetManager
    {
        OpenGLAssetManager();

        void loadAssetManifest(const questart::AssetManifest& assetManifest);

        const questart::OpenGLPipeline& getPipeline(const questart::assets::Pipeline& pipeline) const;

        const questart::OpenGLMesh& getStaticMesh(const questart::assets::StaticMesh& staticMesh) const;

        const questart::OpenGLTexture& getTexture(const questart::assets::Texture& texture) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart