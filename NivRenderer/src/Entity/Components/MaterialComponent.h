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

    std::string& GetDiffusePath() { return m_DiffusePath; }
    std::string& GetSpecularPath() { return m_SpecularPath; }
    std::string& GetNormalPath() { return m_NormalPath; }
    Ref<TextureAsset>& GetDiffuseTextureAsset() { return m_DiffuseTextureAsset; }
    Ref<TextureAsset>& GetSpecularTextureAsset() { return m_SpecularTextureAsset; }
    Ref<TextureAsset>& GetNormalTextureAsset() { return m_NormalTextureAsset; }
    bool& GetFlipDiffuseTexture() { return m_FlipDiffuseTexture; }
    bool& GetFlipSpecularTexture() { return m_FlipSpecularTexture; }
    bool& GetFlipNormalTexture() { return m_FlipNormalTexture; }

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