#include "MaterialProxy.h"

MaterialProxy::MaterialProxy(const uint32_t id) :
    Proxy(id), m_DiffuseTexture(nullptr), m_NormalTexture(nullptr), m_MetallicTexture(nullptr),
    m_RoughnessTexture(nullptr), m_AOTexture(nullptr), m_EmissiveTexture(nullptr)
{

}

bool MaterialProxy::HasNormalTexture() const
{
    return m_NormalTexture != nullptr;
}

void MaterialProxy::BindDiffuseTexture(const int32_t slot) const
{
    m_DiffuseTexture->BindToSlot(slot);
}

void MaterialProxy::BindNormalTexture(const int32_t slot) const
{
    m_NormalTexture->BindToSlot(slot);
}

void MaterialProxy::BindMetallicTexture(const int32_t slot) const
{
    m_MetallicTexture->BindToSlot(slot);
}

void MaterialProxy::BindRoughnessTexture(const int32_t slot) const
{
    m_RoughnessTexture->BindToSlot(slot);
}

void MaterialProxy::BindAOTexture(const int32_t slot) const
{
    m_AOTexture->BindToSlot(slot);
}

void MaterialProxy::BindEmissiveTexture(const int32_t slot) const
{
    m_EmissiveTexture->BindToSlot(slot);
}

Ref<TextureProxy>& MaterialProxy::GetDiffuseTexture() { return m_DiffuseTexture; }

Ref<TextureProxy>& MaterialProxy::GetNormalTexture() { return m_NormalTexture; }

Ref<TextureProxy>& MaterialProxy::GetMetallicTexture() { return m_MetallicTexture; }

Ref<TextureProxy>& MaterialProxy::GetRoughnessTexture() { return m_RoughnessTexture; }

Ref<TextureProxy>& MaterialProxy::GetAOTexture() { return m_AOTexture; }

Ref<TextureProxy>& MaterialProxy::GetEmissiveTexture() { return m_EmissiveTexture; }
