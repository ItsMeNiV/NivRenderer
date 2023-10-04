#include "Entity/Assets/TextureAsset.h"

#include "AssetManager.h"

TextureAsset::TextureAsset(const uint32_t id, const std::string& path, bool flipVertical, bool loadOnlyOneChannel, int channelIndex) :
    m_Id(id), m_TextureData(nullptr), m_FlipVertical(flipVertical), m_LoadOnlyOneChannel(loadOnlyOneChannel), m_Width(0),
    m_Height(0), m_NrComponents(0), m_ChannelIndex(channelIndex), m_Path(path), m_IsUnloaded(true)
{
}

TextureAsset::~TextureAsset()
{
    if (!m_IsUnloaded)
        delete[] m_TextureData;
}

uint32_t TextureAsset::GetId() const
{
    return m_Id;
}

unsigned char* TextureAsset::GetTextureData() const
{
    return m_TextureData;
}

void TextureAsset::SetTextureData(const unsigned char* const textureData)
{
    if (!m_IsUnloaded)
        delete[] m_TextureData;
    const size_t dataSize = sizeof(unsigned char) * m_Width * m_Height * m_NrComponents;
    m_TextureData = new unsigned char[dataSize];
    memcpy(m_TextureData, textureData, dataSize);

    m_IsUnloaded = false;
}

const bool& TextureAsset::GetFlipVertical() const
{
    return m_FlipVertical;
}

const bool& TextureAsset::GetLoadOnlyOneChannel() const
{
    return m_LoadOnlyOneChannel;
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

int* TextureAsset::GetChannelIndex()
{
    return &m_ChannelIndex;
}

std::string& TextureAsset::GetPath()
{
    return m_Path;
}

bool TextureAsset::isUnloaded() const
{
    return m_IsUnloaded;
}

void TextureAsset::UnloadData()
{
    delete[] m_TextureData;
    m_IsUnloaded = true;
}

void TextureAsset::ReloadData()
{
    AssetManager::GetInstance().ReloadTexture(this);
    m_IsUnloaded = false;
}

std::vector<std::pair<std::string, Property>> TextureAsset::GetAssetProperties()
{
    std::vector<std::pair<std::string, Property>> returnVector;
    return returnVector;
}

nlohmann::ordered_json TextureAsset::SerializeObject()
{
    nlohmann::ordered_json texture = {
        {"Id", m_Id},
        {"InternalPath", m_Path},
        {"FlipVertical", m_FlipVertical},
        {"Width", m_Width},
        {"Height", m_Height},
        {"LoadOnlyOneChannel", m_LoadOnlyOneChannel},
        {"NrComponents", m_NrComponents},
        {"ChannelIndex", m_ChannelIndex}
    };

    return texture;
}

void TextureAsset::DeSerializeObject(nlohmann::json jsonObject)
{
    m_Width = jsonObject["Width"];
    m_Height = jsonObject["Height"];
    m_NrComponents = jsonObject["NrComponents"];
    m_LoadOnlyOneChannel = jsonObject["LoadOnlyOneChannel"];
    m_ChannelIndex = jsonObject["ChannelIndex"];
    ReloadData();
}
