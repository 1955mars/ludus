#include "static-mesh-instance.hpp"
#include <vector>

using questart::StaticMeshInstance;

struct StaticMeshInstance::Internal
{
    const questart::assets::StaticModel model;
    const questart::assets::StaticMesh mesh;
    const questart::assets::Texture texture;
    const glm::mat4 identity;

    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotationAxis;
    float rotationDegrees;
    glm::mat4 transformMatrix;
    StaticMeshUniform meshUniform;

    Internal(const questart::assets::StaticModel& model,
             const questart::assets::StaticMesh& mesh,
             const questart::assets::Texture& texture,
             const glm::vec3& position,
             const glm::vec3& scale,
             const glm::vec3& rotationAxis,
             const float& rotationDegrees)
        : model(model), 
          mesh(mesh),
          texture(texture),
          identity(glm::mat4{1.0f}),
          position(position),
          scale(scale),
          rotationAxis(rotationAxis),
          rotationDegrees(rotationDegrees),
          transformMatrix(identity)
    {
    }

    void update(const std::vector<glm::mat4>& viewMatrix, const std::vector<glm::mat4>& projectionMatrix)
    {
        meshUniform.transformMatrix = glm::translate(identity, position) *
                                      glm::rotate(identity, glm::radians(rotationDegrees), rotationAxis) *
                                      glm::scale(identity, scale);

#if defined (QUEST)

        meshUniform.viewMatrix[0] = viewMatrix[0];
        meshUniform.viewMatrix[1] = viewMatrix[1];
        meshUniform.projectionMatrix[0] = projectionMatrix[0];
        meshUniform.projectionMatrix[1] = projectionMatrix[1];
#else
        meshUniform.viewMatrix = viewMatrix[0];
        meshUniform.projectionMatrix = projectionMatrix[0];
        transformMatrix = projectionMatrix[0] * viewMatrix[0] * meshUniform.transformMatrix;
#endif

    }

    void rotateBy(const float& degrees)
    {
        rotationDegrees += degrees;

        if (rotationDegrees > 360.0f)
        {
            rotationDegrees -= 360.0f;
        }
        else if (rotationDegrees < -360.0f)
        {
            rotationDegrees += 360.0f;
        }
    }
};

StaticMeshInstance::StaticMeshInstance(
    const questart::assets::StaticModel& staticModel,
    const questart::assets::StaticMesh& staticMesh,
    const questart::assets::Texture& texture,
    const glm::vec3& position,
    const glm::vec3& scale,
    const glm::vec3& rotationAxis,
    const float& rotationDegrees)
    : internal(questart::make_internal_ptr<Internal>(
          staticModel,
          staticMesh,
          texture,
          position,
          scale,
          rotationAxis,
          rotationDegrees)) {}

void StaticMeshInstance::update(const std::vector<glm::mat4>& viewMatrix, const std::vector<glm::mat4>& projectionMatrix)
{
    internal->update(viewMatrix, projectionMatrix);
}

void StaticMeshInstance::rotateBy(const float& degrees)
{
    internal->rotateBy(degrees);
}

questart::assets::StaticMesh StaticMeshInstance::getMesh() const
{
    return internal->mesh;
}

questart::assets::Texture StaticMeshInstance::getTexture() const
{
    return internal->texture;
}

const questart::StaticMeshUniform& StaticMeshInstance::getMeshUniformMatrix() const
{
    return internal->meshUniform;
}

const glm::mat4& StaticMeshInstance::getTransformMatrix() const
{
    return internal->transformMatrix;
}