#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "asset-loader.hpp"
#include "vertex.hpp"
#include "log.hpp"
#include "engine.hpp"
#include <string>
#include <sstream>
#include <tiny_obj_loader.h>
#include <unordered_map>
#include <vector>
#include <fstream>


std::string questart::assets::loadTextFile(const std::string& path) {
    return nullptr;
}

questart::Mesh questart::assets::loadOBJFile(const std::string& path)
{
    //std::istringstream sourceStream(questart::assets::loadTextFile(path));
    std::vector<char> buffer{loadBinaryFile(path)};
    std::istringstream sourceStream(buffer.data(), buffer.size());

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
    std::unordered_map<questart::Vertex, uint32_t> uniqueVertices;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            // Construct a new (x, y, z) position for the current mesh index.
            glm::vec3 position{
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2]};

            // Construct a new (u, v) texture coordinate for the current mesh index.
            glm::vec2 texCoord{
                attributes.texcoords[2 * index.texcoord_index + 0],
                1.0f - attributes.texcoords[2 * index.texcoord_index + 1]};

            // Construct a vertex with the extracted data.
            questart::Vertex vertex{position, texCoord};

            // This will help deduplicate vertices - we maintain a hash map where a
            // vertex is used as a unique key with its value being which index can
            // be used to locate the vertex. The vertex is only added if it has not
            // been added before.
            if (uniqueVertices.count(vertex) == 0)
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
                //questart::log("Vertex: ", std::to_string(vertex.position.x) + "," + std::to_string(vertex.position.y) + "," + std::to_string(vertex.position.z));
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }

    questart::log("loadOBJFile: ", "Num of Vertices: " + std::to_string(vertices.size()));

    return questart::Mesh{vertices, indices};
}

std::vector<char> questart::assets::loadBinaryFile(const std::string&path)
{
    questart::log("LoadBinaryFie: ", "Opening the path " + path);

    AAsset* file = AAssetManager_open(questart::Engine::appHandle->activity->assetManager, path.c_str(), AASSET_MODE_UNKNOWN);

    size_t fileLength = AAsset_getLength(file);

    questart::log("LoadBinaryFie: ", "Filelength: " + std::to_string(fileLength));

    std::vector<char> buffer(fileLength);
    AAsset_read(file, buffer.data(), fileLength);
    questart::log("LoadBinaryFie: ", "After read!");
    AAsset_close(file);
    return buffer;
}
