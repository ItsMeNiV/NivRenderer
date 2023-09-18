#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"

class SkyboxProxy : public Proxy
{
public:
    SkyboxProxy(uint32_t id) : Proxy(id), m_Texture(UINT32_MAX)
    {
        float skyboxVertices[] = {// positions
                                  -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                                  1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                                  -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                                  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                                  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                                  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                                  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                                  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                                  -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                                  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                                  -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                                  1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};

        glCreateVertexArrays(1, &m_VertexArray);
        glCreateBuffers(1, &m_VertexBuffer);
        glVertexArrayVertexBuffer(m_VertexArray, 0, m_VertexBuffer, 0, 3 * sizeof(float));

        glNamedBufferStorage(m_VertexBuffer, sizeof(skyboxVertices), &skyboxVertices, GL_DYNAMIC_STORAGE_BIT);

        glEnableVertexArrayAttrib(m_VertexArray, 0);
        glVertexArrayAttribFormat(m_VertexArray, 0, 3, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayAttribBinding(m_VertexArray, 0, 0);
    }

    ~SkyboxProxy() override
    {
        glDeleteBuffers(1, &m_VertexBuffer);
        glDeleteVertexArrays(1, &m_VertexArray);
        glDeleteTextures(1, &m_Texture);
    }

    void SetTextures(const std::array<TextureAsset*, 6>& textures)
    {
        if (m_Texture != UINT32_MAX)
            glDeleteTextures(1, &m_Texture);

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &m_Texture);
        GLenum format;
        GLenum sizedFormat;
        switch (*textures[0]->GetNrComponents())
        {
        case 1:
            format = GL_RED;
            sizedFormat = GL_R16;
            break;
        case 3:
            format = GL_RGB;
            sizedFormat = GL_RGB16;
            break;
        case 4:
        default:
            format = GL_RGBA;
            sizedFormat = GL_RGBA16;
            break;
        }

        glTextureStorage2D(m_Texture, 1, sizedFormat, *textures[0]->GetWidth(), *textures[0]->GetHeight());
        uint8_t i = 0;
        for (auto& textureAsset : textures)
        {
            glTextureSubImage3D(m_Texture, 0, 0, 0, i, *textureAsset->GetWidth(), *textureAsset->GetHeight(), 1, format,
                                GL_UNSIGNED_BYTE, textureAsset->GetTextureData());
            i++;
        }
        glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    void Bind() const
    {
        glBindVertexArray(m_VertexArray);
    }
    void Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void BindTexture(const int32_t slot) const { 
        glBindTextureUnit(slot, m_Texture);
    }

    bool HasAllTexturesSet()
    {
        return m_Texture != UINT32_MAX;
    }

private:
    // TODO: Abstract
    uint32_t m_Texture;
    uint32_t m_VertexBuffer, m_VertexArray;
};