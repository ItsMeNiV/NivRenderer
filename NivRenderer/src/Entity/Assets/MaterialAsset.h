#pragma once
#include "Base.h"
#include "Entity/PropertyType.h"
#include "Entity/Asset.h"
#include "Entity/Assets/TextureAsset.h"

class MaterialAsset : public Asset
{
public:
    MaterialAsset(uint32_t id, const std::string& name);
    ~MaterialAsset() override = default;

    std::string& GetName() { return m_Name; }
    bool GetDirtyFlag() const { return m_DirtyFlag; }
    void SetDirtyFlag(bool dirtyFlag) { m_DirtyFlag = dirtyFlag; }

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

    std::vector<std::pair<std::string, Property>> GetAssetProperties() override;

    ordered_json SerializeObject() override;

private:
    std::string m_Name;
    bool m_DirtyFlag;

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
