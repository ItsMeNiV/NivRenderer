#pragma once
#include "Base.h"
#include "Entity/Component.h"
#include "Entity/Assets/AssetManager.h"

class MaterialComponent : public Component
{
public:
    MaterialComponent();
    MaterialComponent(std::string &&diffusePath);
    MaterialComponent(std::string &&diffusePath, std::string &&specularPath);
    MaterialComponent(std::string &&diffusePath, std::string &&specularPath, std::string&& normalPath);
    ~MaterialComponent() = default;

    std::unordered_map<std::string, ComponentProperty> GetComponentProperties() override;

    const std::string& GetDiffusePath() const { return m_DiffusePath; }
    const std::string& GetSpecularPath() const { return m_SpecularPath; }
    const std::string& GetNormalPath() const { return m_NormalPath; }
    const Ref<TextureAsset>& GetDiffuseTextureAsset() const { return m_DiffuseTextureAsset; }
    const Ref<TextureAsset>& GetSpecularTextureAsset() const { return m_SpecularTextureAsset; }
    const Ref<TextureAsset>& GetNormalTextureAsset() const { return m_NormalTextureAsset; }

private:
    std::string m_DiffusePath;
    Ref<TextureAsset> m_DiffuseTextureAsset;
    bool m_FlipDiffuseTexture;
    std::string m_SpecularPath;
    Ref<TextureAsset> m_SpecularTextureAsset;
    bool m_FlipSpecularTexture;
    std::string m_NormalPath;
    Ref<TextureAsset> m_NormalTextureAsset;
    bool m_FlipNormalTexture;

    void reloadDiffuseTexture();
    void reloadSpecularTexture();
    void reloadNormalTexture();
};