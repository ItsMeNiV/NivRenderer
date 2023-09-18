#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "Entity/Assets/TextureAsset.h"

class TextureProxy : public Proxy
{
public:
    TextureProxy(const uint32_t id);
    ~TextureProxy();

    void CreateTextureFromAsset(TextureAsset* const textureAsset) const;

    void BindToSlot(uint32_t slot) const;

private:
    //TODO: Abstract
    uint32_t m_TextureId;
};