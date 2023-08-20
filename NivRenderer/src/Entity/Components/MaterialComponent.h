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
    ~MaterialComponent() = default;

    std::unordered_map<std::string, ComponentProperty> GetComponentProperties() override;

    const std::string& GetDiffusePath() const { return m_DiffusePath; }
    const std::string& GetSpecularPath() const { return m_SpecularPath; }
    const Ref<TextureAsset>& GetDiffuseTextureAsset() const { return m_DiffuseTextureAsset; }
    const Ref<TextureAsset>& GetSpecularTextureAsset() const { return m_SpecularTextureAsset; }

private:
    std::string m_DiffusePath;
    Ref<TextureAsset> m_DiffuseTextureAsset;
    bool m_FlipDiffuseTexture;
    std::string m_SpecularPath;
    Ref<TextureAsset> m_SpecularTextureAsset;
    bool m_FlipSpecularTexture;

    void reloadDiffuseTexture();
    void reloadSpecularTexture();
};