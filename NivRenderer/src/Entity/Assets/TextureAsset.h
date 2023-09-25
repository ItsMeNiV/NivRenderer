#pragma once
#include "Base.h"
#include "Entity/Asset.h"
#include "Entity/PropertyType.h"

class TextureAsset : public Asset
{
public:
    TextureAsset(const uint32_t id, const std::string& path, bool flipVertical, bool loadOnlyOneChannel, int channelIndex);

    ~TextureAsset();

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

    std::vector<std::pair<std::string, Property>> GetAssetProperties() override;

    ordered_json SerializeObject() override;
    void DeSerializeObject(json jsonObject) override;

private:
    unsigned char* m_TextureData;
    bool m_FlipVertical, m_LoadOnlyOneChannel;
    int m_Width, m_Height, m_NrComponents, m_ChannelIndex;
    std::string m_Path;

    bool m_IsUnloaded;
};
