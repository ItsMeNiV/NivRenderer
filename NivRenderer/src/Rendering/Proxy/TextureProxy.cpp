#include "TextureProxy.h"

TextureProxy::TextureProxy(const uint32_t id)
    : Proxy(id)
{
    glGenTextures(1, &m_TextureId);
}

TextureProxy::~TextureProxy()
{
}

void TextureProxy::CreateTextureFromAsset(TextureAsset* const textureAsset)
{
    GLenum format;
    switch (*textureAsset->GetNrComponents())
    {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
    default:
        format = GL_RGBA;
        break;
    }
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, format, *textureAsset->GetWidth(), *textureAsset->GetHeight(), 0, format,
                 GL_UNSIGNED_BYTE, textureAsset->GetTextureData());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureProxy::BindToSlot(uint32_t slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
}
