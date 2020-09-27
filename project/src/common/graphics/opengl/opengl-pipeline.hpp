#pragma once

#include "../../core/internal_ptr.hpp"
#include "../../core/static-mesh-instance.hpp"
#include <string>
#include <vector>

namespace questart
{
    struct OpenGLAssetManager;

    struct OpenGLPipeline
    {
        OpenGLPipeline(const std::string& shaderName);

        void render(
            const questart::OpenGLAssetManager& assetManager,
            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart