#include "Texture.h"

#include "stb_image.h"

Texture::Texture(std::string&& path, bool flipVertically)
    : m_Paths({ path }), m_TextureType(TextureType::TEXTURE_2D)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    glCreateTextures(m_TextureType, 1, &m_TextureId);

    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(m_Paths[0].c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        const int format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        const int sizedFormat = nrChannels == 3 ? GL_RGB16 : GL_RGBA16;
        glTextureStorage2D(m_TextureId, 1, sizedFormat, width, height);
        glTextureSubImage2D(m_TextureId, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(m_TextureId);
    }
    else
    {
        SPDLOG_DEBUG("Failed to load texture at path:" + m_Paths[0]);
    }
    stbi_image_free(data);
}

Texture::Texture(std::vector<std::string> paths, bool flipVertically)
    : m_Paths(paths), m_TextureType(TextureType::TEXTURE_CUBEMAP)
{
    stbi_set_flip_vertically_on_load(flipVertically);

    glCreateTextures(m_TextureType, 1, &m_TextureId);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < paths.size(); i++)
    {
        unsigned char* data = stbi_load(paths[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            const int format = nrChannels == 3 ? GL_RGB : GL_RGBA;
            const int sizedFormat = nrChannels == 3 ? GL_RGB16 : GL_RGBA16;
            if (i == 0)
                glTextureStorage2D(m_TextureId, 1, sizedFormat, width, height);
            glTextureSubImage3D(m_TextureId, 1, 0, 0, i, width, height, 1, format, GL_UNSIGNED_BYTE, data);

            stbi_image_free(data);
        }
        else
        {
            SPDLOG_DEBUG("Failed to load texture at path:" + paths[i]);
            stbi_image_free(data);
        }
    }

    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

Texture::Texture(const int width, const int height, const InternalFormat internalFormat, const int sampleCount)
    : m_TextureType(sampleCount > 1 ? TextureType::TEXTURE_2D_MULTI : TextureType::TEXTURE_2D), m_TextureId(UINT_MAX)
{
    glCreateTextures(m_TextureType, 1, &m_TextureId);
    if (sampleCount > 1)
    {
        glTextureStorage2DMultisample(m_TextureId, sampleCount, GL_RGBA16F, width, height, GL_TRUE);
    }
    else
    {
        glTextureStorage2D(m_TextureId, 1, internalFormat, width, height);
        glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    if (internalFormat == DEPTH)
    {
        glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        constexpr float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        glTextureParameterfv(m_TextureId, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TextureId);
}

void Texture::ActivateForSlot(uint32_t slot)
{
    assert(slot <= 32);
    glBindTextureUnit(slot, m_TextureId);
}