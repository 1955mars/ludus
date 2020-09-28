#include <log.hpp>
#include "scene-ovr.hpp"
#include "asset-inventory.hpp"
#include "static-mesh-instance.hpp"
#include "asset-manifest.hpp"
#include "glm-wrapper.hpp"

using questart::SceneOVR;
using questart::assets::Pipeline;
using questart::assets::StaticMesh;
using questart::assets::Texture;

namespace
{
    glm::vec3 toGlm(const ovrVector3f& vec)
    {
        return glm::vec3(vec.x, vec.y, vec.z);
    }

    glm::mat4 toGlm(const ovrMatrix4f& matrix)
    {
        return glm::transpose(glm::make_mat4(&matrix.M[0][0]));
    }
}

struct SceneOVR::Internal
{
    std::vector<questart::StaticMeshInstance> staticMeshes;

    Internal(){
    }

    questart::AssetManifest getAssetManifest()
    {
        return questart::AssetManifest{
                {Pipeline::Default},
                {StaticMesh::Crate, StaticMesh::Torus},
                {Texture::Crate, Texture::RedCrossHatch}
        };
    }

    void prepare()
    {

        staticMeshes.push_back(questart::StaticMeshInstance{
                StaticMesh::Crate,           // Mesh
                Texture::Crate,              // Texture
                glm::vec3{0.4f, 0.6f, 0.0f}, // Position
                glm::vec3{0.6f, 0.6f, 0.6f}, // Scale
                glm::vec3{0.0f, 0.4f, 0.9f}, // Rotation axis
                0.0f});                      // Initial rotation

        staticMeshes.push_back(questart::StaticMeshInstance{
                StaticMesh::Torus,            // Mesh
                Texture::RedCrossHatch,       // Texture
                glm::vec3{-0.6f, 0.4f, 0.0f}, // Position
                glm::vec3{0.4f, 0.4f, 0.4f},  // Scale
                glm::vec3{0.2f, 1.0f, 0.4f},  // Rotation axis
                0.0f});                       // Initial rotation

        staticMeshes.push_back(questart::StaticMeshInstance{
                StaticMesh::Crate,             // Mesh
                Texture::Crate,                // Texture
                glm::vec3{-0.5f, -0.5f, 0.0f}, // Position
                glm::vec3{0.7f, 0.3f, 0.3f},   // Scale
                glm::vec3{0.2f, 0.6f, 0.1f},   // Rotation axis
                90.0f});                       // Initial rotation

        staticMeshes.push_back(questart::StaticMeshInstance{
                StaticMesh::Torus,            // Mesh
                Texture::RedCrossHatch,       // Texture
                glm::vec3{0.6f, -0.4f, 0.0f}, // Position
                glm::vec3{0.4f, 0.4f, 0.4f},  // Scale
                glm::vec3{0.6f, 0.3f, 0.1f},  // Rotation axis
                50.0f});

    }

    void update(const float& delta, void* tracking)
    {
        ovrTracking2* trackingInfo = (ovrTracking2*) tracking;


        std::vector<glm::mat4> vM = {::toGlm(trackingInfo->Eye[0].ViewMatrix) , ::toGlm(trackingInfo->Eye[1].ViewMatrix)};
        std::vector<glm::mat4> pM = {::toGlm(trackingInfo->Eye[0].ProjectionMatrix) , ::toGlm(trackingInfo->Eye[1].ProjectionMatrix)};

        for (auto& staticMesh : staticMeshes)
        {
            //questart::log("displaytime: ", std::to_string(delta));
            staticMesh.rotateBy(delta * 50);
            staticMesh.update(vM, pM);
        }
    }

    void render(questart::Renderer& renderer)
    {
        renderer.render(Pipeline::Default, staticMeshes);
    }

};

SceneOVR::SceneOVR()
    :internal(questart::make_internal_ptr<Internal>())
{
}

questart::AssetManifest SceneOVR::getAssetManifest()
{
    return internal->getAssetManifest();
}

void SceneOVR::prepare()
{
    return internal->prepare();
}

void SceneOVR::update(const float& delta, void* tracking)
{
    return internal->update(delta,  tracking);
}

void SceneOVR::render(questart::Renderer& renderer)
{
    return internal->render(renderer);
}

void SceneOVR::onWindowResized(const questart::WindowSize& size)
{

}
