#include "vulkan-pipeline.hpp"
#include "../../../core/asset-inventory.hpp"
#include "../../../core/asset-loader.hpp"
#include "../../../core/vertex.hpp"
#include "vulkan-mesh.hpp"
#include "vulkan-texture.hpp"
#include "vulkan-asset-manager.hpp"
#include <unordered_map>

using questart::VulkanPipeline;

namespace
{
    // The default shader will have one descriptor for texture mapping which
    // will be made available in the fragment shader pipeline stage only. Note
    // that this pipeline does not include a descriptor set for vertex data as
    // we will use Vulkan push constants for this instead of uniform buffers.

    vk::UniqueDescriptorSetLayout createUniformBufferDescriptorSetLayout(const questart::VulkanDevice& device)
    {
        vk::DescriptorSetLayoutBinding uniformBinding{
            0,
            vk::DescriptorType::eUniformBuffer,
            1,
            vk::ShaderStageFlagBits::eVertex,
            nullptr};

        vk::DescriptorSetLayoutCreateInfo createInfoUniforms{
            vk::DescriptorSetLayoutCreateFlags(),
            1,
            &uniformBinding};

        return device.getDevice().createDescriptorSetLayoutUnique(createInfoUniforms);
    }

    vk::UniqueDescriptorSetLayout createSamplerDescriptorSetLayout(const questart::VulkanDevice& device)
    {

        vk::DescriptorSetLayoutBinding textureBinding{
            0,                                         // Binding
            vk::DescriptorType::eCombinedImageSampler, // Descriptor type
            1,                                         // Descriptor count
            vk::ShaderStageFlagBits::eFragment,        // Shader stage flags
            nullptr};                                  // Immutable samplers

        vk::DescriptorSetLayoutCreateInfo createInfoTexture{
            vk::DescriptorSetLayoutCreateFlags(), // Flags
            1,                                    // Binding count
            &textureBinding};                     // Bindings

        return device.getDevice().createDescriptorSetLayoutUnique(createInfoTexture);
    }

    vk::UniquePipelineLayout createPipelineLayout(const questart::VulkanDevice& device,
                                                  const vk::UniqueDescriptorSetLayout& uniformSetLayout,
                                                  const vk::UniqueDescriptorSetLayout& samplerSetLayout)
    {
        // We use push constants for delivering the vertex MVP data to the shader. This is
        // a lightweight alternative to using Vulkan uniform buffers with the caveat that
        // we can only push data up to a certain size, which the minimum guaranteed Vulkan
        // spec claims is 128 bytes. Note that this definition directly correlates to the
        // push constant in the shader code and must match up. A 4x4 matrix will consume
        // 16 floating point values, each of which is 4 bytes making a total of 64 bytes
        // which is half of our 128 byte quota. If we ever needed to push more than 128
        // bytes of data into our shader we must instead implement Vulkan uniform buffers
        // which require more setup work and are less performant than push constants.
      /*  vk::PushConstantRange pushConstantRange{
            vk::ShaderStageFlagBits::eAllGraphics, // Flags
            0,                                     // Offset
            sizeof(glm::mat4)};                    // Size*/

            // We use the descriptor set layout combined with our definition of how we intend
            // to use push constants during the pipeline to produce an overall pipeline layout.

        std::array<vk::DescriptorSetLayout, 2> layouts{
            uniformSetLayout.get(),
            samplerSetLayout.get()};


        vk::PipelineLayoutCreateInfo info{
            vk::PipelineLayoutCreateFlags(), // Flags
            static_cast<uint32_t>(layouts.size()),                               // Layout count
            layouts.data(),            // Layouts,
            0,                               // Push constant range count,
            nullptr               // Push constant ranges
        };

        return device.getDevice().createPipelineLayoutUnique(info);
    }

    vk::UniquePipeline createPipeline(const questart::VulkanPhysicalDevice& physicalDevice,
                                      const questart::VulkanDevice& device,
                                      const vk::PipelineLayout& pipelineLayout,
                                      const std::string& shaderName,
                                      const vk::Viewport& viewport,
                                      const vk::Rect2D& scissor,
                                      const vk::RenderPass& renderPass)
    {
        // Create a vertex shader module from asset file.
        vk::UniqueShaderModule vertexShaderModule{
            device.createShaderModule(questart::assets::loadBinaryFile("assets/shaders/vulkan/" + shaderName + ".vert"))};

        // Describe how to use the vertex shader module in the pipeline.
        vk::PipelineShaderStageCreateInfo vertexShaderInfo{
            vk::PipelineShaderStageCreateFlags(), // Flags
            vk::ShaderStageFlagBits::eVertex,     // Shader stage
            vertexShaderModule.get(),             // Shader module
            "main",                               // Name
            nullptr};                             // Specialisation info

        // Define the data format that will be passed into the vertex shader.
        vk::VertexInputBindingDescription vertexBindingDescription{
            0,                           // Binding
            sizeof(questart::Vertex),         // Stride
            vk::VertexInputRate::eVertex // Input rate
        };

        // Define the shape of the vertex position (x, y, z) attribute.
        vk::VertexInputAttributeDescription vertexPositionDescription{
            0,                                // Location
            0,                                // Binding
            vk::Format::eR32G32B32Sfloat,     // Format
            offsetof(questart::Vertex, position)}; // Offset

        // Define the shape of the texture coordinate (u, v) attribute.
        vk::VertexInputAttributeDescription textureCoordinateDescription{
            1,                                // Location
            0,                                // Binding
            vk::Format::eR32G32Sfloat,        // Format
            offsetof(questart::Vertex, texCoord)}; // Offset

        // Collate all the vertex shader attributes that will be used in the pipeline.
        std::array<vk::VertexInputAttributeDescription, 2> vertexAttributeDescriptions{
            vertexPositionDescription,
            textureCoordinateDescription};

        // Bundle up the collated descriptions defining how vertex data will be passed into the shader.
        vk::PipelineVertexInputStateCreateInfo vertexInputState{
            vk::PipelineVertexInputStateCreateFlags(),                 // Flags
            1,                                                         // Vertex binding description count
            &vertexBindingDescription,                                 // Vertex binding description
            static_cast<uint32_t>(vertexAttributeDescriptions.size()), // Vertex attribute descriptions
            vertexAttributeDescriptions.data()};                       // Vertex attribute descriptions

        // Create a fragment shader module from asset file.
        vk::UniqueShaderModule fragmentShaderModule{
            device.createShaderModule(questart::assets::loadBinaryFile("assets/shaders/vulkan/" + shaderName + ".frag"))};

        // Describe how to use the fragment shader module in the pipeline.
        vk::PipelineShaderStageCreateInfo fragmentShaderInfo{
            vk::PipelineShaderStageCreateFlags(), // Flags
            vk::ShaderStageFlagBits::eFragment,   // Shader stage
            fragmentShaderModule.get(),           // Shader module
            "main",                               // Name
            nullptr};                             // Specialisation info

        // Collate both vertex and fragment shaders into the list of pipeline shaders to use.
        std::array<vk::PipelineShaderStageCreateInfo, 2> stages{
            vertexShaderInfo,
            fragmentShaderInfo};

        // Define what variety of data will be sent into the pipeline - for us a triangle list.
        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{
            vk::PipelineInputAssemblyStateCreateFlags(), // Flags
            vk::PrimitiveTopology::eTriangleList,        // Topology
            0};                                          // Primitive restart enable

        // Declare the viewport and scissor to apply to the shader output.
        vk::PipelineViewportStateCreateInfo viewportState{
            vk::PipelineViewportStateCreateFlags(), // Flags
            1,                                      // Viewport count
            &viewport,                              // Viewports
            1,                                      // Scissor count
            &scissor};                              // Scissors

#if defined(QUEST)
        vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eFront;
#else
        vk::CullModeFlagBits cullMode = vk::CullModeFlagBits::eBack;
#endif
        // Define how the pipeline should process output during rendering.
        vk::PipelineRasterizationStateCreateInfo rasterizationState{
            vk::PipelineRasterizationStateCreateFlags(), // Flags
            VK_FALSE,                                    // Depth clamp enable
            VK_FALSE,                                    // Rasterizer discard enable
            vk::PolygonMode::eFill,                      // Polygon mode
            cullMode,                 // Cull mode
            vk::FrontFace::eCounterClockwise,            // Front face
            VK_FALSE,                                    // Depth bias enable
            0.0f,                                        // Depth bias constant factor
            0.0f,                                        // Depth bias clamp
            0.0f,                                        // Depth bias slop factor
            1.0f};                                       // Line width

        // Define how to apply multi sampling to the shader output.
        vk::PipelineMultisampleStateCreateInfo multisampleState{
            vk::PipelineMultisampleStateCreateFlags(), // Flags
            physicalDevice.getMultiSamplingLevel(),    // Rasterization samples
            VK_FALSE,                                  // Sample shading enabled
            0.0f,                                      // Min sample shading
            nullptr,                                   // Sample mask
            VK_FALSE,                                  // Alpha to coverage enable
            VK_FALSE};                                 // Alpha to one enable

        // If our physical device can do multisampling at the shader level, enable it.
        if (physicalDevice.isShaderMultiSamplingSupported())
        {
            multisampleState.sampleShadingEnable = VK_TRUE;
            multisampleState.minSampleShading = 0.2f;
        }

        // Determine the way that depth testing will be performed for the pipeline.
        vk::PipelineDepthStencilStateCreateInfo depthStencilState{
            vk::PipelineDepthStencilStateCreateFlags(), // Flags
            VK_TRUE,                                    // Depth test enable
            VK_TRUE,                                    // Depth write enable
            vk::CompareOp::eLess,                       // Depth compare operation
            VK_FALSE,                                   // Depth bounds test enable
            VK_FALSE,                                   // Stencil test enable
            vk::StencilOpState(),                       // Stencil front operation
            vk::StencilOpState(),                       // Stencil back operation
            0.0f,                                       // Min depth bounds
            0.0f};                                      // Max depth bounds

        // Define how colors should be written during blending.
        vk::ColorComponentFlags colorWriteMask{
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA};

        // Define how colors should blend together during rendering.
        vk::PipelineColorBlendAttachmentState colorBlendAttachment{
            VK_TRUE,                            // Blend enable
            vk::BlendFactor::eSrcAlpha,         // Source color blend factor
            vk::BlendFactor::eOneMinusSrcAlpha, // Destination color blend factor
            vk::BlendOp::eAdd,                  // Color blend operation
            vk::BlendFactor::eOne,              // Source alpha blend factor
            vk::BlendFactor::eZero,             // Destination alpha blend factor
            vk::BlendOp::eAdd,                  // Alpha blend operation
            colorWriteMask};                    // Color write mask

        // Take the blending attachment and collate it into a pipeline state definition.
        vk::PipelineColorBlendStateCreateInfo colorBlendState{
            vk::PipelineColorBlendStateCreateFlags(), // Flags
            VK_FALSE,                                 // Logic operation enable
            vk::LogicOp::eClear,                      // Logic operation
            1,                                        // Attachment count
            &colorBlendAttachment,                    // Attachments
            {{0, 0, 0, 0}}};                          // Blend constants

        // Collate all the components into a single graphics pipeline definition.
        vk::GraphicsPipelineCreateInfo pipelineCreateInfo{
            vk::PipelineCreateFlags(),            // Flags
            static_cast<uint32_t>(stages.size()), // Stage count (vertex + fragment)
            stages.data(),                        // Stages
            &vertexInputState,                    // Vertex input state
            &inputAssemblyState,                  // Input assembly state
            nullptr,                              // Tesselation state
            &viewportState,                       // Viewport state
            &rasterizationState,                  // Rasterization state
            &multisampleState,                    // Multi sample state
            &depthStencilState,                   // Depth stencil state
            &colorBlendState,                     // Color blend state
            nullptr,                              // Dynamic state
            pipelineLayout,                       // Pipeline layout
            renderPass,                           // Render pass
            0,                                    // Subpass
            vk::Pipeline(),                       // Base pipeline handle
            0};                                   // Base pipeline index

        return device.getDevice().createGraphicsPipelineUnique(nullptr, pipelineCreateInfo);
    }

    vk::UniqueDescriptorPool createDescriptorPool(const questart::VulkanDevice& device)
    {
        const uint32_t maxDescriptors{64};

        vk::DescriptorPoolSize uniformBuffersPoolSize{
            vk::DescriptorType::eUniformBuffer,
            maxDescriptors};

        vk::DescriptorPoolSize combinedImageSamplerPoolSize{
            vk::DescriptorType::eCombinedImageSampler, // Type
            maxDescriptors};                           // Max descriptor count

        std::array<vk::DescriptorPoolSize, 2> poolSizes{uniformBuffersPoolSize, combinedImageSamplerPoolSize};

        vk::DescriptorPoolCreateInfo info{
            vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet, // Flags
            maxDescriptors,                                       // Max sets
            static_cast<uint32_t>(poolSizes.size()),              // Pool size count
            poolSizes.data()};                                    // Pool sizes

        return device.getDevice().createDescriptorPoolUnique(info);
    }



    vk::UniqueDescriptorSet createTextureSamplerDescriptorSet(const questart::VulkanDevice& device,
                                                              const vk::DescriptorPool& descriptorPool,
                                                              const vk::DescriptorSetLayout& descriptorSetLayout,
                                                              const questart::VulkanTexture& texture)
    {
        vk::DescriptorSetAllocateInfo createInfo{
            descriptorPool,        // Descriptor pool
            1,                     // Descriptor set count
            &descriptorSetLayout}; // Descriptor set layouts

        vk::UniqueDescriptorSet descriptorSet{
            std::move(device.getDevice().allocateDescriptorSetsUnique(createInfo)[0])};

        vk::DescriptorImageInfo imageInfo{
            texture.getSampler(),                     // Sampler
            texture.getImageView().getImageView(),    // Image view
            vk::ImageLayout::eShaderReadOnlyOptimal}; // Image layout

        vk::WriteDescriptorSet writeInfo{
            descriptorSet.get(),                       // Destination set
            0,                                         // Destination binding
            0,                                         // Destination array element
            1,                                         // Descriptor count
            vk::DescriptorType::eCombinedImageSampler, // Descriptor type
            &imageInfo,                                // Image info
            nullptr,                                   // Buffer info
            nullptr};                                  // Texel buffer view

        device.getDevice().updateDescriptorSets(1, &writeInfo, 0, nullptr);

        return descriptorSet;
    }

    vk::UniqueDescriptorSet createUniformBufferDescriptorSet(const questart::VulkanDevice& device,
                                                              const vk::DescriptorPool& descriptorPool,
                                                              const vk::DescriptorSetLayout& descriptorSetLayout,
                                                              const questart::VulkanBuffer& buffer)
    {
        vk::DescriptorSetAllocateInfo createInfo{
            descriptorPool,        // Descriptor pool
            1,                     // Descriptor set count
            &descriptorSetLayout}; // Descriptor set layouts

        vk::UniqueDescriptorSet descriptorSet{
            std::move(device.getDevice().allocateDescriptorSetsUnique(createInfo)[0])};

        vk::DescriptorBufferInfo bufferInfo{
            buffer.getBuffer(),
            0,
            sizeof(glm::mat4)};

        vk::WriteDescriptorSet writeInfo{
            descriptorSet.get(),                       // Destination set
            0,                                         // Destination binding
            0,                                         // Destination array element
            1,                                         // Descriptor count
            vk::DescriptorType::eUniformBuffer,        // Descriptor type
            nullptr,                                   // Image info
            &bufferInfo,                               // Buffer info
            nullptr};                                  // Texel buffer view

        device.getDevice().updateDescriptorSets(1, &writeInfo, 0, nullptr);

        return descriptorSet;
    }


} // namespace

struct VulkanPipeline::Internal
{
    const questart::VulkanPhysicalDevice& physicalDevice;
    const questart::VulkanDevice& device;
    const vk::UniqueDescriptorSetLayout uniformBufferDescriptorSetLayout;
    const vk::UniqueDescriptorSetLayout imageSamplerDescriptorSetLayout;
    const vk::UniquePipelineLayout pipelineLayout;
    const vk::UniquePipeline pipeline;
    const vk::UniqueDescriptorPool descriptorPool;
    std::unordered_map<questart::assets::Texture, vk::UniqueDescriptorSet> textureSamplerDescriptorSets;
    std::unordered_map<int, questart::VulkanBuffer> staticMeshUniformBuffers;
    std::unordered_map<int, vk::UniqueDescriptorSet> staticMeshUniformDescriptorSets;

    Internal(const questart::VulkanPhysicalDevice& physicalDevice,
             const questart::VulkanDevice& device,
             const std::string& shaderName,
             const vk::Viewport& viewport,
             const vk::Rect2D& scissor,
             const vk::RenderPass& renderPass)
        : physicalDevice(physicalDevice),
          device(device),
          uniformBufferDescriptorSetLayout(::createUniformBufferDescriptorSetLayout(device)),
          imageSamplerDescriptorSetLayout(::createSamplerDescriptorSetLayout(device)),
          pipelineLayout(::createPipelineLayout(device, uniformBufferDescriptorSetLayout, imageSamplerDescriptorSetLayout)),
          pipeline(::createPipeline(physicalDevice,
                                    device,
                                    pipelineLayout.get(),
                                    shaderName,
                                    viewport,
                                    scissor,
                                    renderPass)),
          descriptorPool(::createDescriptorPool(device)) {}

    const vk::DescriptorSet& getTextureSamplerDescriptorSet(const questart::VulkanDevice& device,
                                                            const questart::VulkanTexture& texture)
    {
        if (textureSamplerDescriptorSets.count(texture.getTextureId()) == 0)
        {
            textureSamplerDescriptorSets.insert(std::make_pair(
                texture.getTextureId(),
                ::createTextureSamplerDescriptorSet(device,
                                                    descriptorPool.get(),
                                                    imageSamplerDescriptorSetLayout.get(),
                                                    texture)));
        }

        return textureSamplerDescriptorSets.at(texture.getTextureId()).get();
    }

    const vk::DescriptorSet& getUniformBufferDescriptorSet(const questart::VulkanDevice& device, 
                                                           const int instanceId)
    {
        /*
        if (staticMeshUniformBuffers.count(instanceId) == 0)
        {
            staticMeshUniformBuffers.insert(std::make_pair(
                instanceId,
                questart::VulkanBuffer(physicalDevice, device, sizeof(glm::mat4), vk::BufferUsageFlagBits::eUniformBuffer,
                                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                                       nullptr)));
        }*/

        const questart::VulkanBuffer& buffer = staticMeshUniformBuffers.at(instanceId);

        if (staticMeshUniformDescriptorSets.count(instanceId) == 0)
        {
            staticMeshUniformDescriptorSets.insert(std::make_pair(
                instanceId,
                ::createUniformBufferDescriptorSet(device, descriptorPool.get(), uniformBufferDescriptorSetLayout.get(), buffer)));
        }

        return staticMeshUniformDescriptorSets.at(instanceId).get();
    }

    void updateUniformBuffer(const int instanceId, const vk::DeviceSize& size, const void* data)
    {
        if (staticMeshUniformBuffers.count(instanceId) == 0)
        {
            staticMeshUniformBuffers.insert(std::make_pair(
                instanceId,
                questart::VulkanBuffer(physicalDevice, device, size, vk::BufferUsageFlagBits::eUniformBuffer,
                                       vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                                       nullptr)));
        }

        questart::VulkanBuffer& buffer = staticMeshUniformBuffers.at(instanceId);
        buffer.copyData(device, size, data);
    }

    void render(const questart::VulkanDevice& device,
                const vk::CommandBuffer& commandBuffer,
                const questart::VulkanAssetManager& assetManager,
                const std::vector<questart::StaticMeshInstance>& staticMeshInstances)
    {
        int instanceId = 0;
        for (const questart::StaticMeshInstance& meshInstance : staticMeshInstances)
        {
            const questart::VulkanMesh& mesh{assetManager.getStaticMesh(meshInstance.getMesh())};
            //const glm::mat4& transform{meshInstance.getTransformMatrix()};
            const questart::StaticMeshUniform& meshUniform{meshInstance.getMeshUniformMatrix()};

            /*commandBuffer.pushConstants(pipelineLayout.get(),
                                        vk::ShaderStageFlagBits::eAllGraphics,
                                        0,
                                        sizeof(glm::mat4),
                                        &transform);*/

            updateUniformBuffer(instanceId, sizeof(questart::StaticMeshUniform), &meshUniform);

            commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.get());

            vk::DeviceSize offsets[]{0};
            commandBuffer.bindVertexBuffers(0, 1, &mesh.getVertexBuffer(), offsets);

            commandBuffer.bindIndexBuffer(mesh.getIndexBuffer(), 0, vk::IndexType::eUint32);

            const questart::VulkanTexture& texture{assetManager.getTexture(meshInstance.getTexture())};

            const vk::DescriptorSet& uniformBufferDescriptorSet{
                getUniformBufferDescriptorSet(device, instanceId)};

            const vk::DescriptorSet& textureSamplerDescriptorSet{
                getTextureSamplerDescriptorSet(device, texture)};


            std::array<vk::DescriptorSet, 2> descriptorSets{
                uniformBufferDescriptorSet,
                textureSamplerDescriptorSet};

            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             pipelineLayout.get(),
                                             0,
                                             static_cast<uint32_t>(descriptorSets.size()), 
                                             descriptorSets.data(),
                                             0, nullptr);

            commandBuffer.drawIndexed(mesh.getNumIndices(), 1, 0, 0, 0);

            instanceId++;
        }
    }
};

VulkanPipeline::VulkanPipeline(const questart::VulkanPhysicalDevice& physicalDevice,
                               const questart::VulkanDevice& device,
                               const std::string& shaderName,
                               const vk::Viewport& viewport,
                               const vk::Rect2D& scissor,
                               const vk::RenderPass& renderPass)
    : internal(questart::make_internal_ptr<Internal>(physicalDevice, device, shaderName, viewport, scissor, renderPass)) {}

void VulkanPipeline::render(const questart::VulkanDevice& device,
                            const vk::CommandBuffer& commandBuffer,
                            const questart::VulkanAssetManager& assetManager,
                            const std::vector<questart::StaticMeshInstance>& staticMeshInstances) const
{
    internal->render(device, commandBuffer, assetManager, staticMeshInstances);
}