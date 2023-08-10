#pragma once
#include "stb_image.h"

class TextureAsset
{
public:
    TextureAsset(bool flipVertical);

    ~TextureAsset();

    unsigned char* GetTextureData() const;
    void SetTextureData(unsigned char* const textureData);
    const bool& GetFlipVertical() const;
    int* GetWidth();
    int* GetHeight();
    int* GetNrComponents();

private:
    unsigned char* m_TextureData;
    bool m_FlipVertical;
    int m_Width, m_Height, m_NrComponents;

};
