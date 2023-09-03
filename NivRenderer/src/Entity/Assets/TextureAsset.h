#pragma once
#include "Base.h"
#include "Asset.h"

class TextureAsset : public Asset
{
public:
    TextureAsset(const uint32_t id, const std::string& path, bool flipVertical);

    ~TextureAsset();

    unsigned char* GetTextureData() const;
    void SetTextureData(const unsigned char* const textureData);
    const bool& GetFlipVertical() const;
    int* GetWidth();
    int* GetHeight();
    int* GetNrComponents();
    const std::string& GetPath();

private:
    unsigned char* m_TextureData;
    bool m_FlipVertical;
    int m_Width, m_Height, m_NrComponents;
    std::string m_Path;
};
