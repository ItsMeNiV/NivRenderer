#include "Entity/Components/MaterialComponent.h"

MaterialComponent::MaterialComponent(): MaterialComponent(std::string("default"), std::string(""))
{}

MaterialComponent::MaterialComponent(std::string&& diffusePath): MaterialComponent(std::move(diffusePath), std::string(""))
{}

MaterialComponent::MaterialComponent(std::string&& diffusePath, std::string&& specularPath):
    Component("MaterialComponent"), m_DiffusePath(std::move(diffusePath)), m_SpecularPath(std::move(specularPath)), m_FlipDiffuseTexture(false), m_FlipSpecularTexture(false)
{
    reloadDiffuseTexture();
    reloadSpecularTexture();
}

std::unordered_map<std::string, ComponentProperty> MaterialComponent::GetComponentProperties()
{
    std::unordered_map<std::string, ComponentProperty> returnMap;

    returnMap["Diffuse Path"] = {NivRenderer::PropertyType::PATH, &m_DiffusePath, [this]() { reloadDiffuseTexture(); }};
    returnMap["Flip Diffuse Texture"] = {NivRenderer::PropertyType::BOOL, &m_FlipDiffuseTexture};
    returnMap["Specular Path"] = {NivRenderer::PropertyType::PATH, &m_SpecularPath, [this]() { reloadSpecularTexture(); }};
    returnMap["Flip Specular Texture"] = {NivRenderer::PropertyType::BOOL, &m_FlipSpecularTexture};

    return returnMap;
}

void MaterialComponent::reloadDiffuseTexture()
{
    if (m_DiffusePath.empty())
        return;

    m_DiffuseTextureAsset = AssetManager::GetInstance().LoadTexture(m_DiffusePath, m_FlipDiffuseTexture);
}

void MaterialComponent::reloadSpecularTexture()
{
    if (m_SpecularPath.empty())
        return;

    m_SpecularTextureAsset = AssetManager::GetInstance().LoadTexture(m_SpecularPath, m_FlipSpecularTexture);
}
