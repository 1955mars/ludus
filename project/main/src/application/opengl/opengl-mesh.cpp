#include "opengl-mesh.hpp"
#include "../../core/glm-wrapper.hpp"
#include "../../core/graphics-wrapper.hpp"
#include <vector>

using questart::OpenGLMesh;

namespace
{
    GLuint createVertexBuffer(const questart::Mesh& mesh)
    {
        std::vector<float> bufferData;

        for (const auto& vertex : mesh.getVertices())
        {
            // Position
            bufferData.push_back(vertex.position.x);
            bufferData.push_back(vertex.position.y);
            bufferData.push_back(vertex.position.z);

            // Texture Coordinates
            bufferData.push_back(vertex.texCoord.x);
            bufferData.push_back(vertex.texCoord.y);
        }

        GLuint bufferId;
        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ARRAY_BUFFER,
                     bufferData.size() * sizeof(float),
                     bufferData.data(),
                     GL_STATIC_DRAW);

        return bufferId;
    }

    GLuint createIndexBuffer(const questart::Mesh& mesh)
    {
        GLuint bufferId;
        glGenBuffers(1, &bufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh.getIndices().size() * sizeof(uint32_t),
                     mesh.getIndices().data(),
                     GL_STATIC_DRAW);

        return bufferId;
    }
} // namespace

struct OpenGLMesh::Internal
{
    const GLuint bufferIdVertices;
    const GLuint bufferIdIndices;
    const uint32_t numIndices;

    Internal(const questart::Mesh& mesh)
        : bufferIdVertices(::createVertexBuffer(mesh)),
          bufferIdIndices(::createIndexBuffer(mesh)),
          numIndices(static_cast<uint32_t>(mesh.getIndices().size())) {}

    ~Internal()
    {
        glDeleteBuffers(1, &bufferIdVertices);
        glDeleteBuffers(1, &bufferIdIndices);
    }
};

OpenGLMesh::OpenGLMesh(const questart::Mesh& mesh)
    : internal(questart::make_internal_ptr<Internal>(mesh)) {}

const GLuint& OpenGLMesh::getVertexBufferId() const
{
    return internal->bufferIdVertices;
}

const GLuint& OpenGLMesh::getIndexBufferId() const
{
    return internal->bufferIdIndices;
}

const uint32_t& OpenGLMesh::getNumIndices() const
{

    return internal->numIndices;
}