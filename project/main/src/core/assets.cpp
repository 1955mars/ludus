#define TINYOBJLOADER_IMPLEMENTATION

#include "assets.hpp"
#include "sdl-wrapper.hpp"
#include "vertex.hpp"
#include <string>
#include <sstream>
#include <tiny_obj_loader.h>
#include <unordered_map>
#include <vector>


std::string questart::assets::loadTextFile(const std::string& path)
{
    SDL_RWops* file{SDL_RWFromFile(path.c_str(), "r")};
    size_t fileLength{static_cast<size_t>(SDL_RWsize(file))};
    void* data{SDL_LoadFile_RW(file, nullptr, 1)};
    std::string result(static_cast<char*>(data), fileLength);
    SDL_free(data);

    return result;
}

questart::Mesh questart::assets::loadOBJFile(const std::string& path)
{
    std::istringstream sourceStream(questart::assets::loadTextFile(path));

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warning;
    std::string error;

    if (!tinyobj::LoadObj(&attributes, &shapes, &materials, 
        &warning, &error, &sourceStream))
    {
        throw std::runtime_error("questart::assets::loadOBJFile: Error: " + warning + error);
    }

    std::vector<questart::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::unordered_map<glm::vec3, uint32_t> uniqueVertices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            glm::vec3 position{
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2]};

            if (uniqueVertices.count(position) == 0)
            {
                uniqueVertices[position] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(questart::Vertex{position});
            }

            indices.push_back(uniqueVertices[position]);
        }
    }

    return questart::Mesh{vertices, indices};
}