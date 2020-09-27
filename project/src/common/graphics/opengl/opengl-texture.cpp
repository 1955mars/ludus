#include "opengl-texture.hpp"
#include "../../core/graphics-wrapper.hpp"
#include "../../core/asset-loader.hpp"
#include <ktx.h>
#include <ktxvulkan.h>

using questart::OpenGLTexture;

namespace
{
    GLuint createTexture(const std::string& texturePath)
    {
        ktxResult result;
        ktxTexture* ktxTexture;

        std::vector<char> buffer{questart::assets::loadBinaryFile(texturePath)};

        result = ktxTexture_CreateFromMemory((ktx_uint8_t*)buffer.data(), buffer.size(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxTexture);

        uint32_t imageWidth = ktxTexture->baseWidth;
        uint32_t imageHeight = ktxTexture->baseHeight;
        //uint32_t imageChannels = ktxTexture->numDimensions;
        uint32_t mipLevels{static_cast<uint32_t>(std::floor(std::log2(std::max(imageWidth, imageHeight)))) + 1};
        ktx_uint8_t* ktxTextureData = ktxTexture_GetData(ktxTexture);

        GLuint textureId;

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            ktxTextureData);
        glGenerateMipmap(GL_TEXTURE_2D);

        return textureId;
    }
} // namespace

struct OpenGLTexture::Internal
{
    const GLuint textureId;

    Internal(const std::string& texturePath)
        : textureId(::createTexture(texturePath)) {}

    ~Internal()
    {
        glDeleteTextures(1, &textureId);
    }
};

OpenGLTexture::OpenGLTexture(const std::string& texturePath)
    : internal(questart::make_internal_ptr<Internal>(texturePath)) {}

void OpenGLTexture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, internal->textureId);
}