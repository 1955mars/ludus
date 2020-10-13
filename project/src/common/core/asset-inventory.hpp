#pragma once

#include <string>

namespace questart::assets
{
    enum class Pipeline
    {
        Default
    };

    enum class StaticModel
    {
        None,
        Helmet,
        Box
    };

    enum class StaticMesh
    {
        None,
        Crate,
        Torus
    };

    enum class Texture
    {
        None,
        Crate,
        RedCrossHatch
    };

    std::string resolvePipelinePath(const questart::assets::Pipeline& pipeline);

    std::string resolveStaticModelPath(const questart::assets::StaticModel& staticModel);

    std::string resolveStaticMeshPath(const questart::assets::StaticMesh& staticMesh);

    std::string resolveTexturePath(const questart::assets::Texture& texture);

} // namespace questart::assets