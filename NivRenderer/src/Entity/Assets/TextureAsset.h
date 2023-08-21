#pragma once
#include "Base.h"

#include "stb_image.h"

class TextureAsset
{
public:
    TextureAsset(const std::string& path, bool flipVertical);

    ~TextureAsset();

    unsigned char* GetTextureData() const;
    void SetTextureData(unsigned char* const textureData);
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
