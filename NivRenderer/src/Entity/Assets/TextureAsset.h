#pragma once
#include "Base.h"
#include "json.hpp"

class TextureAsset
{
public:
    TextureAsset(const uint32_t id, const std::string& path, bool flipVertical, bool loadOnlyOneChannel, int channelIndex);

    ~TextureAsset();

    uint32_t GetId() const;
    unsigned char* GetTextureData() const;
    void SetTextureData(const unsigned char* const textureData);
    const bool& GetFlipVertical() const;
    const bool& GetLoadOnlyOneChannel() const;
    int* GetWidth();
    int* GetHeight();
    int* GetNrComponents();
    int* GetChannelIndex();
    std::string& GetPath();

    bool isUnloaded() const;
    void UnloadData();
    void ReloadData();

    nlohmann::ordered_json SerializeObject();
    void DeSerializeObject(nlohmann::json jsonObject);

private:
    uint32_t m_Id;
    unsigned char* m_TextureData;
    bool m_FlipVertical, m_LoadOnlyOneChannel;
    int m_Width, m_Height, m_NrComponents, m_ChannelIndex;
    std::string m_Path;

    bool m_IsUnloaded;
};
