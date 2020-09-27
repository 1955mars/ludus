#pragma once

#include <string>

namespace questart::assets
{
    enum class Pipeline
    {
        Default
    };

    enum class StaticMesh
    {
        Crate,
        Torus
    };

    enum class Texture
    {
        Crate,
        RedCrossHatch
    };

    std::string resolvePipelinePath(const questart::assets::Pipeline& pipeline);

    std::string resolveStaticMeshPath(const questart::assets::StaticMesh& staticMesh);

    std::string resolveTexturePath(const questart::assets::Texture& texture);

} // namespace questart::assets