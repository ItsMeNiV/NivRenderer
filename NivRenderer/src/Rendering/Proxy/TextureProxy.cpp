#include "TextureProxy.h"

TextureProxy::TextureProxy(const uint32_t id)
    : Proxy(id)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
}

TextureProxy::~TextureProxy() = default;

void TextureProxy::CreateTextureFromAsset(TextureAsset* const textureAsset) const
{
    /*if (textureAsset->isUnloaded()) TODO
        textureAsset->ReloadData();*/ 

    GLenum format;
    GLenum sizedFormat;
    switch (textureAsset->nrComponents)
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

    glTextureStorage2D(m_TextureId, 1, sizedFormat, textureAsset->width, textureAsset->height);
    glTextureSubImage2D(m_TextureId, 0, 0, 0, textureAsset->width, textureAsset->height, format,
                        GL_UNSIGNED_BYTE, textureAsset->textureData);
    glGenerateTextureMipmap(m_TextureId);

    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //textureAsset->UnloadData(); TODO
}

void TextureProxy::BindToSlot(uint32_t slot) const
{
    glBindTextureUnit(slot, m_TextureId);
}
