#include "vulkan-model.hpp"
#include "vulkan-buffer.hpp"

using questart::VulkanModel;


struct VulkanModel::Internal
{
    struct Material
    {
        enum AlphaMode { ALPHAMODE_OPAQUE, ALPHAMODE_MASK, ALPHAMODE_BLEND };
        AlphaMode alphaMode = ALPHAMODE_OPAQUE;
        float alphaCutoff = 1.0f;
        float metallicFactor = 1.0f;
        float roughnessFactor = 1.0f;
        glm::vec4 baseColorFactor = glm::vec4(1.0f);
        questart::VulkanTexture baseColorTexture;
        questart::VulkanTexture metallicRoughnessTexture;
        questart::VulkanTexture normalTexture;
        questart::VulkanTexture occlusionTexture;
        questart::VulkanTexture emissiveTexture;
        questart::VulkanTexture specularGlossinessTexture;
        questart::VulkanTexture diffuseTexture;
    };

    struct Primitive
    {
        uint32_t firstIndex;
        uint32_t indexCount;
        uint32_t firstVertex;
        uint32_t vertexCount;
        Material& material;

        struct Dimensions {
            glm::vec3 min = glm::vec3(FLT_MAX);
            glm::vec3 max = glm::vec3(-FLT_MAX);
            glm::vec3 size;
            glm::vec3 center;
            float radius;
        } dimensions;

        void setDimensions(glm::vec3 min, glm::vec3 max)
        {
            dimensions.min = min;
            dimensions.max = max;
            dimensions.size = max - min;
            dimensions.center = (min + max) / 2.0f;
            dimensions.radius = glm::distance(min, max) / 2.0f;
        }
        Primitive(uint32_t firstIndex, uint32_t indexCount, Material& material)
            : firstIndex(firstIndex), indexCount(indexCount), material(material){};

    };

    struct Mesh
    {
        std::vector<Primitive*> primitives;
        std::string name;
        questart::VulkanBuffer uniformBuffer;

        struct UniformBlock{
            glm::mat4 matrix;
            glm::mat4 jointMatrix[64]{};
            float jointcount{ 0 };
        }uniformBlock;
        

    };

    Internal(const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const questart::VulkanCommandPool& commandPool,
             const std::string& path)
    {
    }

};


VulkanModel::VulkanModel(const questart::VulkanPhysicalDevice& physicalDevice,
                         const questart::VulkanDevice& device,
                         const questart::VulkanCommandPool& commandPool,
                         const std::string& path)
                         :internal(make_internal_ptr<Internal>(physicalDevice, device, commandPool, path))
{
}