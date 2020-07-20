#pragma once

#include "mesh.hpp"
#include <string>

namespace questart::assets
{
    std::string loadTextFile(const std::string& path);

    questart::Mesh loadOBJFile(const std::string& path);
} // namespace questart::assets