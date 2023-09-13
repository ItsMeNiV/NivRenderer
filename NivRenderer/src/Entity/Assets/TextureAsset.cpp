#include "Entity/Assets/TextureAsset.h"

TextureAsset::TextureAsset(const uint32_t id, const std::string& path, bool flipVertical) :
    Asset(id), m_TextureData(nullptr), m_FlipVertical(flipVertical), m_Width(0), m_Height(0), m_NrComponents(0), m_Path(path)
{
}

TextureAsset::~TextureAsset()
{
    delete[] m_TextureData;
}

unsigned char* TextureAsset::GetTextureData() const
{
    return m_TextureData;
}

void TextureAsset::SetTextureData(const unsigned char* const textureData)
{
    delete[] m_TextureData;
    const size_t dataSize = sizeof(unsigned char) * m_Width * m_Height * m_NrComponents;
    m_TextureData = new unsigned char[dataSize];
    memcpy(m_TextureData, textureData, dataSize);
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

std::vector<std::pair<std::string, Property>> TextureAsset::GetAssetProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;
    return returnVector;
}

ordered_json TextureAsset::SerializeObject()
{
    ordered_json texture = {
        {"Id", GetId()},
        {"InternalPath", m_Path},
        {"FlipVertical", m_FlipVertical},
        {"Width", m_Width},
        {"Height", m_Height},
        {"NrComponents", m_NrComponents}
    };
    texture["TextureData"] = json::array_t();
    auto textureData = texture["TextureData"];
    const uint32_t dataSize = m_Width * m_Height * m_NrComponents;
    std::vector<unsigned char> data(dataSize);
    memcpy(data.data(), m_TextureData, dataSize);
    texture["TextureData"] = data;

    return texture;
}

void TextureAsset::DeSerializeObject(json jsonObject)
{
    m_Width = jsonObject["Width"];
    m_Height = jsonObject["Height"];
    m_NrComponents = jsonObject["NrComponents"];
    const std::vector<unsigned char> data = jsonObject["TextureData"];
    delete[] m_TextureData;
    const size_t dataSize = sizeof(unsigned char) * m_Width * m_Height * m_NrComponents;
    m_TextureData = new unsigned char[dataSize];
    memcpy(m_TextureData, data.data(), dataSize);
}
