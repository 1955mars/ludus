#pragma once

#include "glm-wrapper.hpp"

namespace questart
{
	struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoord;

        bool operator==(const questart::Vertex& other) const;
    };
} // namespace questart

namespace std
{
    template <>
    struct hash<questart::Vertex>
    {
        size_t operator()(const questart::Vertex& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)) >> 1);
        }
    };
} // namespace std