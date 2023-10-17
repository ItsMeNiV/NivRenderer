#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "Assets/Assets.h"

class TextureProxy : public Proxy
{
public:
    TextureProxy(const uint32_t id);
    ~TextureProxy();

    void CreateTextureFromAsset(TextureAsset* const textureAsset) const;

    void BindToSlot(uint32_t slot) const;
    uint32_t GetTextureId() const { return m_TextureId; }

private:
    //TODO: Abstract
    uint32_t m_TextureId;
};