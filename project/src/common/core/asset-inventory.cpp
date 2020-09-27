#include "asset-inventory.hpp"

std::string questart::assets::resolvePipelinePath(const questart::assets::Pipeline& pipeline)
{
    switch (pipeline)
    {
        case questart::assets::Pipeline::Default:
            return "default";
    }
}

std::string questart::assets::resolveStaticMeshPath(const questart::assets::StaticMesh& staticMesh)
{
    switch (staticMesh)
    {
        case questart::assets::StaticMesh::Crate:
            return "assets/models/crate.obj";
        case questart::assets::StaticMesh::Torus:
            return "assets/models/torus.obj";
    }
}

std::string questart::assets::resolveTexturePath(const questart::assets::Texture& texture)
{
    switch (texture)
    {
        case questart::assets::Texture::Crate:
            return "assets/textures/crate.ktx";
        case questart::assets::Texture::RedCrossHatch:
            return "assets/textures/red_cross_hatch.ktx";
    }
}