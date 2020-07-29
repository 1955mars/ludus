#pragma once

#include "asset-inventory.hpp"
#include "static-mesh-instance.hpp"
#include <vector>

namespace questart
{
    struct Renderer
    {
        virtual void render(
            const questart::assets::Pipeline& pipeline,
            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) = 0;
    };
} // namespace ast