#include "Entity/Assets/TextureAsset.h"

TextureAsset::TextureAsset(const std::string& path, bool flipVertical) :
    m_TextureData(nullptr), m_FlipVertical(flipVertical), m_Width(0), m_Height(0), m_NrComponents(0), m_Path(path)
{
}

TextureAsset::~TextureAsset()
{
    stbi_image_free(m_TextureData);
}

unsigned char* TextureAsset::GetTextureData() const
{
    return m_TextureData;
}

void TextureAsset::SetTextureData(unsigned char* const textureData)
{
    m_TextureData = textureData;
}

const bool& TextureAsset::GetFlipVertical() const
{
    return m_FlipVertical;
}

int* TextureAsset::GetWidth()
{
    return &m_Width;
}

int* TextureAsset::GetHeight()
{
    return &m_Height;
}

int* TextureAsset::GetNrComponents()
{
    return &m_NrComponents;
}

const std::string& TextureAsset::GetPath()
{
    return m_Path;
}
