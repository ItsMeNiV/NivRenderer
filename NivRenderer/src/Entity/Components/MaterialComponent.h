#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/AssetManager.h"

class MaterialComponent : public Component
{
public:
    MaterialComponent();
    ~MaterialComponent() = default;

    std::vector<std::pair<std::string, ComponentProperty>> GetComponentProperties() override;

    std::string& GetDiffusePath() { return m_DiffusePath; }
    std::string& GetNormalPath() { return m_NormalPath; }
    std::string& GetMetallicPath() { return m_MetallicPath; }
    std::string& GetRoughnessPath() { return m_RoughnessPath; }
    std::string& GetAOPath() { return m_AOPath; }
    std::string& GetEmissivePath() { return m_EmissivePath; }
    Ref<TextureAsset>& GetDiffuseTextureAsset() { return m_DiffuseTextureAsset; }
    Ref<TextureAsset>& GetNormalTextureAsset() { return m_NormalTextureAsset; }
    Ref<TextureAsset>& GetMetallicTextureAsset() { return m_MetallicTextureAsset; }
    Ref<TextureAsset>& GetRoughnessTextureAsset() { return m_RoughnessTextureAsset; }
    Ref<TextureAsset>& GetAOTextureAsset() { return m_AOTextureAsset; }
    Ref<TextureAsset>& GetEmissiveTextureAsset() { return m_EmissiveTextureAsset; }
    bool& GetFlipDiffuseTexture() { return m_FlipDiffuseTexture; }
    bool& GetFlipNormalTexture() { return m_FlipNormalTexture; }
    bool& GetFlipMetallicTexture() { return m_FlipMetallicTexture; }
    bool& GetFlipRoughnessTexture() { return m_FlipRoughnessTexture; }
    bool& GetFlipAOTexture() { return m_FlipAOTexture; }
    bool& GetFlipEmissiveTexture() { return m_FlipEmissiveTexture; }

private:
    // Diffuse/Albedo
    std::string m_DiffusePath;
    Ref<TextureAsset> m_DiffuseTextureAsset;
    bool m_FlipDiffuseTexture;
    // Normal
    std::string m_NormalPath;
    Ref<TextureAsset> m_NormalTextureAsset;
    bool m_FlipNormalTexture;
    // Metallic
    std::string m_MetallicPath;
    Ref<TextureAsset> m_MetallicTextureAsset;
    bool m_FlipMetallicTexture;
    // Roughness
    std::string m_RoughnessPath;
    Ref<TextureAsset> m_RoughnessTextureAsset;
    bool m_FlipRoughnessTexture;
    // AO
    std::string m_AOPath;
    Ref<TextureAsset> m_AOTextureAsset;
    bool m_FlipAOTexture;
    // Emissive
    std::string m_EmissivePath;
    Ref<TextureAsset> m_EmissiveTextureAsset;
    bool m_FlipEmissiveTexture;

    void reloadDiffuseTexture();
    void reloadNormalTexture();
    void reloadMetallicTexture();
    void reloadRoughnessTexture();
    void reloadAOTexture();
    void reloadEmissiveTexture();
};