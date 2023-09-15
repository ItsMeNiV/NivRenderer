#pragma once
#include "Base.h"
#include "Rendering/Proxy/Proxy.h"
#include "TextureProxy.h"
#include "Entity/Components/MaterialComponent.h"

class MaterialProxy : public Proxy
{
public:
    MaterialProxy(const uint32_t id);

    bool HasNormalTexture() const;
    void BindDiffuseTexture(const int32_t slot) const;
    void BindNormalTexture(const int32_t slot) const;
    void BindMetallicTexture(const int32_t slot) const;
    void BindRoughnessTexture(const int32_t slot) const;
    void BindAOTexture(const int32_t slot) const;
    void BindEmissiveTexture(const int32_t slot) const;

    TextureProxy** GetDiffuseTexture();
    TextureProxy** GetNormalTexture();
    TextureProxy** GetMetallicTexture();
    TextureProxy** GetRoughnessTexture();
    TextureProxy** GetAOTexture();
    TextureProxy** GetEmissiveTexture();

private:
    TextureProxy* m_DiffuseTexture, *m_NormalTexture, *m_MetallicTexture, *m_RoughnessTexture, *m_AOTexture, *m_EmissiveTexture;
};
