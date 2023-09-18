#include "TextureProxy.h"

TextureProxy::TextureProxy(const uint32_t id)
    : Proxy(id)
{
    glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureId);
}

TextureProxy::~TextureProxy() = default;

void TextureProxy::CreateTextureFromAsset(TextureAsset* const textureAsset) const
{
    GLenum format;
    GLenum sizedFormat;
    switch (*textureAsset->GetNrComponents())
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

    glTextureStorage2D(m_TextureId, 1, sizedFormat, *textureAsset->GetWidth(), *textureAsset->GetHeight());
    glTextureSubImage2D(m_TextureId, 0, 0, 0, *textureAsset->GetWidth(), *textureAsset->GetHeight(), format,
                        GL_UNSIGNED_BYTE, textureAsset->GetTextureData());
    glGenerateTextureMipmap(m_TextureId);

    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(m_TextureId, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTextureParameteri(m_TextureId, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void TextureProxy::BindToSlot(uint32_t slot) const
{
    glBindTextureUnit(slot, m_TextureId);
}
