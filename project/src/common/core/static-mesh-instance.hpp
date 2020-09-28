#pragma once

#include "asset-inventory.hpp"
#include "glm-wrapper.hpp"
#include "internal_ptr.hpp"

namespace questart
{
    struct StaticMeshUniform
    {
#if defined(QUEST)
        glm::mat4 viewMatrix[2];
        glm::mat4 projectionMatrix[2];
#else
        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;
#endif
        glm::mat4 transformMatrix;
    };

    struct StaticMeshInstance
    {
        StaticMeshInstance(const questart::assets::StaticMesh& staticMesh,
                           const questart::assets::Texture& texture,
                           const glm::vec3& position = glm::vec3{0.0f, 0.0f, 0.0f},
                           const glm::vec3& scale = glm::vec3{1.0f, 1.0f, 1.0f},
                           const glm::vec3& rotationAxis = glm::vec3{0.0f, 1.0f, 0.0f},
                           const float& rotationDegrees = 0.0f);

        void update(const std::vector<glm::mat4>& viewMatrix, const std::vector<glm::mat4>& projectionMatrix);

        void rotateBy(const float& degrees);

        questart::assets::StaticMesh getMesh() const;

        questart::assets::Texture getTexture() const;

        const questart::StaticMeshUniform& getMeshUniformMatrix() const;

        const glm::mat4& getTransformMatrix() const;

    private:
        struct Internal;
        questart::internal_ptr<Internal> internal;
    };
} // namespace questart