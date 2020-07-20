#pragma once

#include "internal_ptr.hpp"
#include "vertex.hpp"
#include <vector>

namespace questart
{
	struct Mesh
    {
        Mesh(const std::vector<questart::Vertex>& vertices, const std::vector<uint32_t>& indices);
        const std::vector<questart::Vertex>& getVertices() const;
        const std::vector<uint32_t>& getIndices() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} //namespace questart